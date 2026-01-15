// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "CssLoader.h"

#include <source_location>

#include <QFile>
#include <QMetaEnum>
#include <QRegularExpression>
#include <QStandardPaths>

#include <Color.h>
#include <Style.h>
#include <StyleRule.h>

#include <CssParser.h>

#include "css_logging.h"

using namespace Qt::StringLiterals;
using namespace std::string_literals;

namespace fs = std::filesystem;

template<typename Target, typename Getter, typename Setter>
struct PropertyGroupBuilder {
    using PropertyGroup = std::remove_pointer_t<std::invoke_result_t<Getter, Target *>>;

    PropertyGroupBuilder(Target *_target, Getter _getter, Setter _setter)
        : target(_target)
        , getter(_getter)
        , setter(_setter)
    {
        instance = std::invoke(getter, target);
        if (!instance) {
            temporary = std::make_unique<PropertyGroup>();
            instance = temporary.get();
        }
    }

    ~PropertyGroupBuilder()
    {
        if (temporary && temporary->hasAnyValue()) {
            std::invoke(setter, target, std::move(temporary));
        }
    }

    inline PropertyGroup *operator->()
    {
        return instance;
    }

    PropertyGroupBuilder &operator=(const PropertyGroup &group)
    {
        *instance = group;
        return *this;
    }

    std::unique_ptr<PropertyGroup> temporary;
    PropertyGroup *instance;
    Target *target;
    Getter getter;
    Setter setter;
};

inline float to_px(const cssparser::Dimension &value)
{
    switch (value.unit) {
    case cssparser::Unit::Px:
        return value.value;
    default:
        return 0.0;
    }
}

inline float to_px(const cssparser::Value &value)
{
    if (!std::holds_alternative<cssparser::Dimension>(value)) {
        return 0.0;
    }

    return to_px(std::get<cssparser::Dimension>(value));
}

Color to_color(const cssparser::Color::Color &color)
{
    switch (color.type) {
    case cssparser::Color::ColorType::Empty: {
        return Color{};
    }
    case cssparser::Color::ColorType::Rgba: {
        auto value = std::get<cssparser::Color::Rgba>(color.data);
        return Color::rgba(value.r, value.g, value.b, value.a);
    }
    case cssparser::Color::ColorType::Custom: {
        auto value = std::get<cssparser::Color::CustomColor>(color.data);
        QStringList arguments;
        std::ranges::transform(value.arguments, std::back_inserter(arguments), QString::fromStdString);
        return Color::custom(QString::fromStdString(value.source), arguments);
    }
    case cssparser::Color::ColorType::Modified: {
        auto modifiedColor = std::get<cssparser::Color::ModifiedColor>(color.data);

        switch (modifiedColor.operation) {
        case cssparser::Color::OperationType::Add: {
            auto other = std::get<std::shared_ptr<cssparser::Color::Color>>(modifiedColor.data);
            return Color::add(to_color(*modifiedColor.color), to_color(*other));
        }
        case cssparser::Color::OperationType::Subtract: {
            auto other = std::get<std::shared_ptr<cssparser::Color::Color>>(modifiedColor.data);
            return Color::subtract(to_color(*modifiedColor.color), to_color(*other));
        }
        case cssparser::Color::OperationType::Multiply: {
            auto other = std::get<std::shared_ptr<cssparser::Color::Color>>(modifiedColor.data);
            return Color::multiply(to_color(*modifiedColor.color), to_color(*other));
        }
        case cssparser::Color::OperationType::Set: {
            auto data = std::get<cssparser::Color::SetOperationData>(modifiedColor.data);
            return Color::set(to_color(*modifiedColor.color), data.r, data.g, data.b, data.a);
        }
        case cssparser::Color::OperationType::Mix: {
            auto data = std::get<cssparser::Color::MixOperationData>(modifiedColor.data);
            return Color::mix(to_color(*modifiedColor.color), to_color(*data.other), data.amount);
        }
        }
    }
    }

    return Color{};
}

inline Color to_color(const cssparser::Value &value)
{
    if (!std::holds_alternative<cssparser::Color::Color>(value)) {
        return Color{};
    }

    return to_color(std::get<cssparser::Color::Color>(value));
}

inline fs::path to_path(const cssparser::Value &value)
{
    if (!std::holds_alternative<cssparser::Url>(value)) {
        return fs::path{};
    }

    auto url = std::get<cssparser::Url>(value);
    return url.data;
}

bool matches_keyword(const cssparser::Value &value, const QString &keyword)
{
    if (!std::holds_alternative<std::string>(value)) {
        return false;
    }

    auto string = QString::fromStdString(std::get<std::string>(value));
    return string.compare(keyword, Qt::CaseInsensitive) == 0;
}

template<class... Ts>
struct overloads : Ts... {
    using Ts::operator()...;
};

inline QVariant to_qvariant(const cssparser::Value &value)
{
    constexpr auto visitor = overloads{
        /* clang-format off */
        [](std::nullopt_t) { return QVariant{}; },
        [](const std::string &v) { return QVariant{QString::fromStdString(v)}; },
        [](int v) { return QVariant::fromValue(v); },
        [](const cssparser::Color::Color &v) { return QVariant::fromValue(to_color(v).toQColor()); },
        [](const cssparser::Dimension &v) { return QVariant::fromValue(to_px(v)); },
        [](const cssparser::Url &v) { return QVariant::fromValue(fs::path(v.data)); },
        /* clang-format on */
    };
    return std::visit(visitor, value);
}

template<typename T>
inline int toEnumIntValue(/* intentional copy */ std::string value)
{
    const auto metaEnum = QMetaEnum::fromType<T>();

    std::erase(value, '-');

    auto count = metaEnum.keyCount();
    for (int i = 0; i < count; ++i) {
        if (qstrnicmp(metaEnum.key(i), value.c_str(), value.size()) == 0) {
            return metaEnum.value(i);
        }
    }

    return -1;
}

template<typename T>
inline T toEnumValue(const std::string &value)
{
    return T{toEnumIntValue<T>(value)};
}

template<typename T>
inline void setImage(T *output, const fs::path &rootPath, const cssparser::Property &property)
{
    PropertyGroupBuilder image(output, &T::image, &T::setImage);

    QImage imageData;
    std::filesystem::path path = rootPath / to_path(property.value());

    if (!imageData.load(QString::fromStdString(path))) {
        qCWarning(UNION_CSS) << "Could not load image" << path.string();
        return;
    }

    image->setImageData(imageData);
    image->setWidth(imageData.width());
    image->setHeight(imageData.height());
    if (property.values.size() > 1) {
        auto value = property.values.at(1);
        if (matches_keyword(value, u"mask"_s)) {
            image->setFlags(ImageFlag::Mask);
        } else if (matches_keyword(value, u"inverted-mask"_s)) {
            image->setFlags(ImageFlag::InvertedMask);
        }
        image->setMaskColor(to_color(property.values.at(2)));
    }
}

template<typename T>
inline void setAlignment(T *output, const cssparser::Property &property)
{
    std::unique_ptr<AlignmentProperty> tempAlignment;
    AlignmentProperty *alignment;
    if (output->alignment()) {
        alignment = output->alignment();
    } else {
        tempAlignment = std::make_unique<AlignmentProperty>();
        alignment = tempAlignment.get();
    }

    if (property.name.ends_with("alignment-container"s)) {
        alignment->setContainer(toEnumValue<AlignmentContainer>(property.value<std::string>()));
    } else if (property.name.ends_with("alignment-horizontal"s)) {
        alignment->setHorizontal(toEnumValue<Alignment>(property.value<std::string>()));
    } else if (property.name.ends_with("alignment-vertical"s)) {
        alignment->setVertical(toEnumValue<Alignment>(property.value<std::string>()));
    } else if (property.name.ends_with("alignment-order"s)) {
        alignment->setOrder(property.value<int>(0));
    } else if (property.values.size() == 4) {
        alignment->setContainer(toEnumValue<AlignmentContainer>(property.value<std::string>(0)));
        alignment->setHorizontal(toEnumValue<Alignment>(property.value<std::string>(1)));
        alignment->setVertical(toEnumValue<Alignment>(property.value<std::string>(2)));
        alignment->setOrder(property.value<int>(3));
    }

    if (tempAlignment && tempAlignment->hasAnyValue()) {
        output->setAlignment(std::move(tempAlignment));
    }
}

inline void sizeFromProperty(SizeProperty *output, const cssparser::Property &property)
{
    if (property.name.ends_with("left")) {
        output->setLeft(to_px(property.value()));
    } else if (property.name.ends_with("right")) {
        output->setRight(to_px(property.value()));
    } else if (property.name.ends_with("top")) {
        output->setTop(to_px(property.value()));
    } else if (property.name.ends_with("bottom")) {
        output->setBottom(to_px(property.value()));
    } else {
        if (property.values.size() == 1) {
            auto value = to_px(property.value());
            output->setLeft(value);
            output->setRight(value);
            output->setTop(value);
            output->setBottom(value);
        } else if (property.values.size() == 2) {
            auto horizontal = to_px(property.value(0));
            auto vertical = to_px(property.value(1));
            output->setLeft(horizontal);
            output->setRight(horizontal);
            output->setTop(vertical);
            output->setBottom(vertical);
        } else if (property.values.size() == 3) {
            output->setTop(to_px(property.value(0)));
            output->setRight(to_px(property.value(1)));
            output->setBottom(to_px(property.value(2)));
            output->setLeft(to_px(property.value(1)));
        } else if (property.values.size() == 4) {
            output->setTop(to_px(property.value(0)));
            output->setRight(to_px(property.value(1)));
            output->setBottom(to_px(property.value(2)));
            output->setLeft(to_px(property.value(3)));
        }
    }
}

template<typename T>
inline void setDirectionValue(T *output, const std::string &baseName, const cssparser::Property &property)
{
    QList<QByteArray> directions;
    QList<QByteArray> properties;
    QList<cssparser::Value> values;

    auto parts = QByteArray(property.name).split('-');
    if (parts.isEmpty() || parts.first() != QByteArrayView(baseName)) {
        qCDebug(UNION_CSS) << "Ignoring invalid property assignment of" << parts << "expected" << baseName;
        return;
    }

    if (parts.size() == 1) {
        directions = {"left", "right", "top", "bottom"};
        properties = {"width", "style", "color"};

        if (property.values.size() == 1 && std::holds_alternative<std::string>(property.value())) {
            values = {std::nullopt, property.value(), std::nullopt};
        } else {
            values = {property.value(0), property.value(1), property.value(2)};
        }
    } else if (parts.size() == 2) {
        auto propertyName = parts.last();
        if (propertyName == "width" || propertyName == "style" || propertyName == "color") {
            directions = {"left", "right", "top", "bottom"};
            properties = {propertyName};
            values = {property.value()};
        } else {
            directions = {propertyName};
            properties = {"width", "style", "color"};

            if (property.values.size() == 1 && matches_keyword(property.value(), u"none"_s)) {
                values = {std::nullopt, property.value(), std::nullopt};
            } else {
                values = {property.value(0), property.value(1), property.value(2)};
            }
        }
    } else {
        directions = {parts.at(1)};
        properties = {parts.at(2)};
        values = {property.value()};
    }

    auto setLineValue = [](auto &&line, const QByteArray &property, const cssparser::Value &value) {
        if (property == "width" || property == "size") {
            line->setSize(to_px(value));
        } else if (property == "style") {
            line->setStyle(toEnumValue<Union::Properties::LineStyle>(std::get<std::string>(value)));
        } else if (property == "color") {
            line->setColor(to_color(value));
        }
    };

    for (const auto &direction : std::as_const(directions)) {
        for (qsizetype i = 0; i < properties.size(); ++i) {
            auto property = properties.at(i);
            auto value = values.at(i);
            if (direction == "left") {
                setLineValue(PropertyGroupBuilder(output, &T::left, &T::setLeft), property, value);
            } else if (direction == "right") {
                setLineValue(PropertyGroupBuilder(output, &T::right, &T::setRight), property, value);
            } else if (direction == "top") {
                setLineValue(PropertyGroupBuilder(output, &T::top, &T::setTop), property, value);
            } else if (direction == "bottom") {
                setLineValue(PropertyGroupBuilder(output, &T::bottom, &T::setBottom), property, value);
            }
        }
    }
}

bool CssLoader::load(Style::Ptr theme)
{
    auto defaultsPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"union/css/defaults"_s, QStandardPaths::LocateDirectory);

    // This is a bit hacky but allows us to provide defaults that the actual
    // style doesn't need to care about. Ultimately we want to do something
    // cleaner here.
    cssparser::StyleSheet styleSheet;
    styleSheet.set_root_path(fs::path(defaultsPath.toStdString()));
    styleSheet.parse_file("default.css"s);

    auto path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"union/css/styles/"_s + theme->name(), QStandardPaths::LocateDirectory);
    m_stylePath = fs::path(path.toStdString());

    styleSheet.set_root_path(m_stylePath);
    styleSheet.parse_file("style.css");

    if (styleSheet.errors().size() > 0) {
        qCWarning(UNION_CSS) << "Errors encountered while parsing CSS:";
        for (const auto &error : styleSheet.errors()) {
            qCWarning(UNION_CSS) << error.message.data();
        }
    }

    for (const auto &rule : styleSheet.rules()) {
        if (rule.properties.empty()) {
            continue;
        }

        auto styleRule = StyleRule::create();
        styleRule->setSelectors(createSelectorList(rule.selector));

        auto properties = std::make_unique<StyleProperty>();
        createProperties(properties.get(), rule.properties);
        styleRule->setProperties(std::move(properties));
        theme->insert(styleRule);
    }

    return true;
}

Union::SelectorList CssLoader::createSelectorList(const cssparser::Selector &selector)
{
    Union::SelectorList result;
    std::ranges::transform(selector.parts, std::back_inserter(result), [this](const auto &part) {
        return createSelector(part);
    });
    return result;
}

Union::Selector CssLoader::createSelector(const cssparser::SelectorPart &part)
{
    switch (part.kind) {
    case cssparser::SelectorKind::Unknown:
    case cssparser::SelectorKind::RelativeParent:
        break;
    case cssparser::SelectorKind::AnyElement:
        return Union::Selector::create<Union::SelectorType::AnyElement>();
    case cssparser::SelectorKind::Type:
        return Union::Selector::create<Union::SelectorType::Type>(QString::fromStdString(std::get<std::string>(part.value)));
    case cssparser::SelectorKind::Id:
        return Union::Selector::create<Union::SelectorType::Id>(QString::fromStdString(std::get<std::string>(part.value)));
    case cssparser::SelectorKind::PseudoClass: {
        auto value = toEnumIntValue<Union::Element::States>(std::get<std::string>(part.value));
        return Union::Selector::create<Union::SelectorType::State>(Union::Element::State{value});
    }
    case cssparser::SelectorKind::Class:
        return Union::Selector::create<Union::SelectorType::Hint>(QString::fromStdString(std::get<std::string>(part.value)));
    case cssparser::SelectorKind::Attribute: {
        auto attributeMatch = part.attributeMatch.value();
        auto name = QString::fromStdString(attributeMatch.name);
        switch (attributeMatch.op) {
        case cssparser::AttributeOperator::Exists:
            return Union::Selector::create<Union::SelectorType::AttributeExists>(name);
        case cssparser::AttributeOperator::Equals:
            return Union::Selector::create<Union::SelectorType::AttributeEquals>(std::make_pair(name, to_qvariant(attributeMatch.value)));
        case cssparser::AttributeOperator::Includes:
        case cssparser::AttributeOperator::Prefixed:
        case cssparser::AttributeOperator::Suffixed:
            // TODO The above should technically not be substring matches
        case cssparser::AttributeOperator::Substring: {
            auto value = QString::fromStdString(std::get<std::string>(attributeMatch.value));
            return Union::Selector::create<Union::SelectorType::AttributeSubstringMatch>(std::make_pair(name, value));
        }
        case cssparser::AttributeOperator::DashMatch:
            // TODO
        case cssparser::AttributeOperator::None:
            break;
        }

        break;
    }
    case cssparser::SelectorKind::DocumentRoot:
        // TODO
        break;
    case cssparser::SelectorKind::DescendantCombinator:
        return Union::Selector::create<Union::SelectorType::DescendantCombinator>();
    case cssparser::SelectorKind::ChildCombinator:
        return Union::Selector::create<Union::SelectorType::ChildCombinator>();
    }

    return Union::Selector::create();
}

void CssLoader::createProperties(StyleProperty *output, const std::vector<cssparser::Property> &properties)
{
    for (auto property : properties) {
        if (property.name == "width"s || property.name == "height"s || property.name == "spacing"s) {
            setLayoutProperty(output, property);
        } else if (property.name.starts_with("padding")) {
            setLayoutProperty(output, property);
        } else if (property.name.starts_with("inset")) {
            setLayoutProperty(output, property);
        } else if (property.name.starts_with("margin")) {
            setLayoutProperty(output, property);
        } else if (property.name.starts_with("layout")) {
            setLayoutProperty(output, property);
        } else if (property.name.starts_with("background")) {
            setBackgroundProperty(output, property);
        } else if (property.name.starts_with("border")) {
            setBorderProperty(output, property);
        } else if (property.name.starts_with("outline")) {
            setOutlineProperty(output, property);
        } else if (property.name.starts_with("text") || property.name.starts_with("font")) {
            setTextProperty(output, property);
        } else if (property.name.starts_with("icon")) {
            setIconProperty(output, property);
        } else if (property.name == "color"s) {
            setTextProperty(output, property);
            setIconProperty(output, property);
        } else if (property.name.starts_with("shadow") || property.name.starts_with("box-shadow")) {
            setShadowProperty(output, property);
        }
    }
}

void CssLoader::setLayoutProperty(StyleProperty *output, const cssparser::Property &property)
{
    PropertyGroupBuilder layout(output, &StyleProperty::layout, &StyleProperty::setLayout);

    if (property.name == "width"s) {
        layout->setWidth(to_px(property.value()));
    } else if (property.name == "height"s) {
        layout->setHeight(to_px(property.value()));
    } else if (property.name == "spacing"s) {
        layout->setSpacing(to_px(property.value()));
    } else if (property.name.starts_with("layout-alignment")) {
        setAlignment(layout.instance, property);
    } else if (property.name.starts_with("padding")) {
        PropertyGroupBuilder padding(layout.instance, &LayoutProperty::padding, &LayoutProperty::setPadding);
        sizeFromProperty(padding.instance, property);
    } else if (property.name.starts_with("inset")) {
        PropertyGroupBuilder inset(layout.instance, &LayoutProperty::inset, &LayoutProperty::setInset);
        sizeFromProperty(inset.instance, property);
    } else if (property.name.starts_with("margin")) {
        PropertyGroupBuilder margins(layout.instance, &LayoutProperty::margins, &LayoutProperty::setMargins);
        sizeFromProperty(margins.instance, property);
    }
}

void CssLoader::setBackgroundProperty(StyleProperty *output, const cssparser::Property &property)
{
    PropertyGroupBuilder background(output, &StyleProperty::background, &StyleProperty::setBackground);

    if (property.name == "background"s) {
        auto value = property.value();
        if (matches_keyword(value, u"none"_s)) {
            background->setColor(Union::Color{});
            background->setImage(ImageProperty::empty());
        } else if (std::holds_alternative<cssparser::Color::Color>(value)) {
            background->setColor(to_color(value));
        } else if (std::holds_alternative<cssparser::Url>(value)) {
            setImage(background.instance, m_stylePath, property);
        }
    }

    if (property.name == "background-color"s) {
        background->setColor(to_color(property.value()));
    }

    if (property.name == "background-image") {
        if (matches_keyword(property.value(), u"none"_s)) {
            background->setImage(ImageProperty::empty());
        } else {
            setImage(background.instance, m_stylePath, property);
        }
    }

    if (property.name == "background-image-mask-color") {
        PropertyGroupBuilder image(background.instance, &BackgroundProperty::image, &BackgroundProperty::setImage);
        image->setMaskColor(to_color(property.value()));
    }
}

void CssLoader::setBorderProperty(StyleProperty *output, const cssparser::Property &property)
{
    PropertyGroupBuilder border(output, &StyleProperty::border, &StyleProperty::setBorder);

    if (property.name.ends_with("radius")) {
        PropertyGroupBuilder corners(output, &StyleProperty::corners, &StyleProperty::setCorners);

        auto setCornerRadius = [](auto &&corner, qreal radius) {
            corner->setRadius(radius);
        };

        if (property.name == "border-radius"s) {
            if (property.values.size() == 1) {
                auto radius = to_px(property.value());
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::topLeft, &CornersProperty::setTopLeft), radius);
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::topRight, &CornersProperty::setTopRight), radius);
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::bottomLeft, &CornersProperty::setBottomLeft), radius);
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::bottomRight, &CornersProperty::setBottomRight), radius);
            } else if (property.values.size() == 4) {
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::topLeft, &CornersProperty::setTopLeft), to_px(property.value(0)));
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::topRight, &CornersProperty::setTopRight), to_px(property.value(1)));
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::bottomRight, &CornersProperty::setBottomRight),
                                to_px(property.value(2)));
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::bottomLeft, &CornersProperty::setBottomLeft),
                                to_px(property.value(3)));
            }
        } else if (property.name == "border-top-left-radius") {
            setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::topLeft, &CornersProperty::setTopLeft), to_px(property.value()));
        } else if (property.name == "border-top-right-radius") {
            setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::topRight, &CornersProperty::setTopRight), to_px(property.value()));
        } else if (property.name == "border-bottom-left-radius") {
            setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::bottomLeft, &CornersProperty::setBottomLeft), to_px(property.value()));
        } else if (property.name == "border-bottom-right-radius") {
            setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::bottomRight, &CornersProperty::setBottomRight), to_px(property.value()));
        }

        return;
    }

    setDirectionValue(border.instance, "border"s, property);
}

void CssLoader::setOutlineProperty(StyleProperty *output, const cssparser::Property &property)
{
    PropertyGroupBuilder outline(output, &StyleProperty::outline, &StyleProperty::setOutline);
    setDirectionValue(outline.instance, "outline"s, property);
}

void CssLoader::setTextProperty(StyleProperty *output, const cssparser::Property &property)
{
    PropertyGroupBuilder text(output, &StyleProperty::text, &StyleProperty::setText);

    if (property.name.starts_with("text-alignment")) {
        setAlignment(text.instance, property);
    }

    if (property.name.starts_with("font")) {
        auto font = text->font().value_or(QFont{});

        if (property.name == "font-family") {
            font.setFamily(QString::fromStdString(property.value<std::string>()));
        } else if (property.name == "font-size") {
            auto dimension = property.value<cssparser::Dimension>();
            switch (dimension.unit) {
            case cssparser::Unit::Px:
                font.setPixelSize(int(dimension.value));
                break;
            case cssparser::Unit::Pt:
                font.setPointSizeF(dimension.value);
                break;
            case cssparser::Unit::Percent:
                font.setPointSizeF(font.pointSizeF() * dimension.value);
                break;
            default:
                qCWarning(UNION_CSS) << "Invalid unit for font-size";
                break;
            }
        } else if (property.name == "font-weight") {
            if (std::holds_alternative<int>(property.value())) {
                font.setWeight(QFont::Weight(property.value<int>()));
            } else {
                auto name = property.value<std::string>();
                if (name == "normal") {
                    font.setWeight(QFont::Weight::Normal);
                } else if (name == "bold") {
                    font.setWeight(QFont::Weight::Bold);
                } else if (name == "bolder") {
                    font.setWeight(QFont::Weight(font.weight() + 100));
                } else if (name == "lighter") {
                    font.setWeight(QFont::Weight(font.weight() - 100));
                }
            }
        }

        text->setFont(font);
    }

    if (property.name == "color" || property.name == "text-color") {
        text->setColor(to_color(property.value()));
    }
}

void CssLoader::setIconProperty(StyleProperty *output, const cssparser::Property &property)
{
    PropertyGroupBuilder icon(output, &StyleProperty::icon, &StyleProperty::setIcon);

    if (property.name.starts_with("icon-alignment")) {
        setAlignment(icon.instance, property);
    }

    if (property.name == "icon-name") {
        icon->setName(QString::fromStdString(property.value<std::string>()));
    } else if (property.name == "icon-width") {
        icon->setWidth(to_px(property.value()));
    } else if (property.name == "icon-height") {
        icon->setHeight(to_px(property.value()));
    } else if (property.name == "icon-size") {
        icon->setWidth(to_px(property.value()));
        icon->setHeight(to_px(property.value()));
    } else if (property.name == "color" || property.name == "icon-color") {
        icon->setColor(to_color(property.value()));
    }
}

void CssLoader::setShadowProperty(StyleProperty *output, const cssparser::Property &property)
{
    if (property.name == "box-shadow") {
        if (matches_keyword(property.value(), u"none"_s)) {
            output->setShadow(ShadowProperty::empty());
        } else {
            PropertyGroupBuilder shadow(output, &StyleProperty::shadow, &StyleProperty::setShadow);

            PropertyGroupBuilder offset(shadow.instance, &ShadowProperty::offset, &ShadowProperty::setOffset);
            offset->setHorizontal(to_px(property.value(0)));
            offset->setVertical(to_px(property.value(1)));

            shadow->setBlur(to_px(property.value(2)));
            shadow->setSize(to_px(property.value(3)));
            shadow->setColor(to_color(property.value(4)));
        }
    }
}
