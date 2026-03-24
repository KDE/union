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
    switch (value.unit()) {
    case cssparser::Dimension::Unit::Px:
        return value.value();
    default:
        return 0.0;
    }
}

inline float to_px(const cssparser::Value &value)
{
    if (value.type() != cssparser::Value::Type::Dimension) {
        return 0.0;
    }

    return to_px(value.get<cssparser::Dimension>());
}

Color to_color(const cssparser::Color::Color &color)
{
    switch (color.type()) {
    case cssparser::Color::Color::Type::Empty: {
        return Color{};
    }
    case cssparser::Color::Color::Type::Rgba: {
        auto value = color.get<cssparser::Color::RgbaData>();
        return Color::rgba(value.r(), value.g(), value.b(), value.a());
    }
    case cssparser::Color::Color::Type::Custom: {
        auto value = color.get<cssparser::Color::CustomColorData>();
        QStringList arguments;
        std::ranges::transform(value.arguments(), std::back_inserter(arguments), QString::fromStdString);
        return Color::custom(QString::fromStdString(value.source()), arguments);
    }
    case cssparser::Color::Color::Type::Modified: {
        auto modifiedColor = color.get<cssparser::Color::ModifiedColorData>();

        switch (modifiedColor.operation()) {
        case cssparser::Color::ModifiedColorData::Operation::Unknown:
            return Color{};
        case cssparser::Color::ModifiedColorData::Operation::Add: {
            auto other = modifiedColor.get<std::shared_ptr<cssparser::Color::Color>>();
            return Color::add(to_color(*modifiedColor.color()), to_color(*other));
        }
        case cssparser::Color::ModifiedColorData::Operation::Subtract: {
            auto other = modifiedColor.get<std::shared_ptr<cssparser::Color::Color>>();
            return Color::subtract(to_color(*modifiedColor.color()), to_color(*other));
        }
        case cssparser::Color::ModifiedColorData::Operation::Multiply: {
            auto other = modifiedColor.get<std::shared_ptr<cssparser::Color::Color>>();
            return Color::multiply(to_color(*modifiedColor.color()), to_color(*other));
        }
        case cssparser::Color::ModifiedColorData::Operation::Set: {
            auto data = modifiedColor.get<cssparser::Color::SetOperationData>();
            return Color::set(to_color(*modifiedColor.color()), data.r(), data.g(), data.b(), data.a());
        }
        case cssparser::Color::ModifiedColorData::Operation::Mix: {
            auto data = modifiedColor.get<cssparser::Color::MixOperationData>();
            return Color::mix(to_color(*modifiedColor.color()), to_color(*data.other()), data.amount());
        }
        }
    }
    }

    return Color{};
}

inline Color to_color(const cssparser::Value &value)
{
    if (value.type() != cssparser::Value::Type::Color) {
        return Color{};
    }

    return to_color(value.get<cssparser::Color::Color>());
}

inline fs::path to_path(const cssparser::Value &value)
{
    if (value.type() != cssparser::Value::Type::Url) {
        return fs::path{};
    }

    return value.get<std::string>();
}

bool matches_keyword(const cssparser::Value &value, const QString &keyword)
{
    if (value.type() != cssparser::Value::Type::String) {
        return false;
    }

    auto string = QString::fromStdString(value.get<std::string>());
    return string.compare(keyword, Qt::CaseInsensitive) == 0;
}

inline QVariant to_qvariant(const cssparser::Value &value)
{
    switch (value.type()) {
    case cssparser::Value::Type::Empty:
        return QVariant{};
    case cssparser::Value::Type::Dimension:
        return to_px(value);
    case cssparser::Value::Type::String:
        return QString::fromStdString(value.get<std::string>());
    case cssparser::Value::Type::Color:
        return QVariant::fromValue(to_color(value));
    case cssparser::Value::Type::Image:
    case cssparser::Value::Type::Url:
        return QVariant::fromValue(to_path(value));
    case cssparser::Value::Type::Integer:
        return value.get<int>();
    }

    return QVariant{};
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

    fs::path path = rootPath / to_path(property.value());
    if (!fs::exists(path)) {
        qCWarning(UNION_CSS) << "Could not load image" << path.string();
        return;
    }

    image->setSource(path);

    if (property.values().size() > 1) {
        auto value = property.value(1);
        if (matches_keyword(value, u"mask"_s)) {
            image->setFlags(ImageFlag::Mask);
        } else if (matches_keyword(value, u"inverted-mask"_s)) {
            image->setFlags(ImageFlag::InvertedMask);
        }
        image->setMaskColor(to_color(property.value(2)));
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

    if (property.name().ends_with("alignment-container"s)) {
        alignment->setContainer(toEnumValue<AlignmentContainer>(property.value<std::string>()));
    } else if (property.name().ends_with("alignment-horizontal"s)) {
        alignment->setHorizontal(toEnumValue<Alignment>(property.value<std::string>()));
    } else if (property.name().ends_with("alignment-vertical"s)) {
        alignment->setVertical(toEnumValue<Alignment>(property.value<std::string>()));
    } else if (property.name().ends_with("alignment-order"s)) {
        alignment->setOrder(property.value<int>(0));
    } else if (property.values().size() == 4) {
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
    if (property.name().ends_with("left")) {
        output->setLeft(to_px(property.value()));
    } else if (property.name().ends_with("right")) {
        output->setRight(to_px(property.value()));
    } else if (property.name().ends_with("top")) {
        output->setTop(to_px(property.value()));
    } else if (property.name().ends_with("bottom")) {
        output->setBottom(to_px(property.value()));
    } else {
        if (property.values().size() == 1) {
            auto value = to_px(property.value());
            output->setLeft(value);
            output->setRight(value);
            output->setTop(value);
            output->setBottom(value);
        } else if (property.values().size() == 2) {
            auto horizontal = to_px(property.value(0));
            auto vertical = to_px(property.value(1));
            output->setLeft(horizontal);
            output->setRight(horizontal);
            output->setTop(vertical);
            output->setBottom(vertical);
        } else if (property.values().size() == 3) {
            output->setTop(to_px(property.value(0)));
            output->setRight(to_px(property.value(1)));
            output->setBottom(to_px(property.value(2)));
            output->setLeft(to_px(property.value(1)));
        } else if (property.values().size() == 4) {
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

    auto parts = QByteArray(property.name()).split('-');
    if (parts.isEmpty() || parts.first() != QByteArrayView(baseName)) {
        qCDebug(UNION_CSS) << "Ignoring invalid property assignment of" << parts << "expected" << baseName;
        return;
    }

    if (parts.size() == 1) {
        directions = {"left", "right", "top", "bottom"};
        properties = {"width", "style", "color"};

        if (property.values().size() == 1 && property.value().type() == cssparser::Value::Type::String) {
            values = {cssparser::Value{}, property.value(), cssparser::Value{}};
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

            if (property.values().size() == 1 && matches_keyword(property.value(), u"none"_s)) {
                values = {cssparser::Value{}, property.value(), cssparser::Value{}};
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
            line->setStyle(toEnumValue<Union::Properties::LineStyle>(value.get<std::string>()));
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
    auto cssPath = fs::path(QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"union/css"_s, QStandardPaths::LocateDirectory).toStdString());

    m_stylePath = cssPath / "styles"s / theme->name().toStdString();
    cssparser::StyleSheet styleSheet(m_stylePath / "style.css"s);

    styleSheet.import(cssPath / "defaults"s / "default.css"s);
    styleSheet.parse();

    const auto paths = styleSheet.paths();
    for (const auto &path : paths) {
        theme->addCachePath(path);
    }

    if (styleSheet.errors().size() > 0) {
        qCWarning(UNION_CSS) << "Errors encountered while parsing CSS:";
        for (const auto &error : styleSheet.errors()) {
            qCWarning(UNION_CSS) << error.message.data();
        }
    }

    for (const auto &rule : styleSheet.rules()) {
        if (rule.properties().empty()) {
            continue;
        }

        auto styleRule = StyleRule::create();
        styleRule->setSelectors(createSelectorList(rule.selector()));

        auto properties = std::make_unique<StyleProperty>();
        createProperties(properties.get(), rule.properties());
        styleRule->setProperties(std::move(properties));
        theme->insert(styleRule);
    }

    return true;
}

Union::SelectorList CssLoader::createSelectorList(const cssparser::Selector &selector)
{
    Union::SelectorList result;
    std::ranges::transform(selector.parts(), std::back_inserter(result), [this](const auto &part) {
        return createSelector(part);
    });
    return result;
}

Union::Selector CssLoader::createSelector(const cssparser::SelectorPart &part)
{
    switch (part.kind()) {
    case cssparser::SelectorPart::Kind::Unknown:
    case cssparser::SelectorPart::Kind::RelativeParent:
    case cssparser::SelectorPart::Kind::CombinatorStart:
        // Ignore
        break;
    case cssparser::SelectorPart::Kind::AnyElement:
        return Union::Selector::create<Union::SelectorType::AnyElement>();
    case cssparser::SelectorPart::Kind::Type:
        return Union::Selector::create<Union::SelectorType::Type>(QString::fromStdString(part.value().get<std::string>()));
    case cssparser::SelectorPart::Kind::Id:
        return Union::Selector::create<Union::SelectorType::Id>(QString::fromStdString(part.value().get<std::string>()));
    case cssparser::SelectorPart::Kind::PseudoClass: {
        auto value = toEnumIntValue<Union::Element::States>(part.value().get<std::string>());
        return Union::Selector::create<Union::SelectorType::State>(Union::Element::State{value});
    }
    case cssparser::SelectorPart::Kind::Class:
        return Union::Selector::create<Union::SelectorType::Hint>(QString::fromStdString(part.value().get<std::string>()));
    case cssparser::SelectorPart::Kind::Attribute: {
        auto attributeMatch = part.attributeMatch().value();
        auto name = QString::fromStdString(attributeMatch.name());
        switch (attributeMatch.op()) {
        case cssparser::AttributeMatch::Operator::Exists:
            return Union::Selector::create<Union::SelectorType::AttributeExists>(name);
        case cssparser::AttributeMatch::Operator::Equals:
            return Union::Selector::create<Union::SelectorType::AttributeEquals>(std::make_pair(name, to_qvariant(attributeMatch.value())));
        case cssparser::AttributeMatch::Operator::Includes:
        case cssparser::AttributeMatch::Operator::Prefixed:
        case cssparser::AttributeMatch::Operator::Suffixed:
            // TODO The above should technically not be substring matches
        case cssparser::AttributeMatch::Operator::Substring: {
            auto value = QString::fromStdString(attributeMatch.value().get<std::string>());
            return Union::Selector::create<Union::SelectorType::AttributeSubstringMatch>(std::make_pair(name, value));
        }
        case cssparser::AttributeMatch::Operator::DashMatch:
            // TODO
        case cssparser::AttributeMatch::Operator::None:
            break;
        }

        break;
    }
    case cssparser::SelectorPart::Kind::DocumentRoot:
        // TODO
        break;
    case cssparser::SelectorPart::Kind::DescendantCombinator:
        return Union::Selector::create<Union::SelectorType::DescendantCombinator>();
    case cssparser::SelectorPart::Kind::ChildCombinator:
        return Union::Selector::create<Union::SelectorType::ChildCombinator>();
    }

    return Union::Selector::create();
}

void CssLoader::createProperties(StyleProperty *output, std::span<const cssparser::Property> properties)
{
    for (auto property : properties) {
        if (property.name() == "width"s || property.name() == "height"s || property.name() == "spacing"s) {
            setLayoutProperty(output, property);
        } else if (property.name().starts_with("padding")) {
            setLayoutProperty(output, property);
        } else if (property.name().starts_with("inset")) {
            setLayoutProperty(output, property);
        } else if (property.name().starts_with("margin")) {
            setLayoutProperty(output, property);
        } else if (property.name().starts_with("layout")) {
            setLayoutProperty(output, property);
        } else if (property.name().starts_with("background")) {
            setBackgroundProperty(output, property);
        } else if (property.name().starts_with("border")) {
            setBorderProperty(output, property);
        } else if (property.name().starts_with("outline")) {
            setOutlineProperty(output, property);
        } else if (property.name().starts_with("text") || property.name().starts_with("font")) {
            setTextProperty(output, property);
        } else if (property.name().starts_with("icon")) {
            setIconProperty(output, property);
        } else if (property.name() == "color"s) {
            setTextProperty(output, property);
            setIconProperty(output, property);
        } else if (property.name().starts_with("shadow") || property.name().starts_with("box-shadow")) {
            setShadowProperty(output, property);
        }
    }
}

void CssLoader::setLayoutProperty(StyleProperty *output, const cssparser::Property &property)
{
    PropertyGroupBuilder layout(output, &StyleProperty::layout, &StyleProperty::setLayout);

    if (property.name() == "width"s) {
        layout->setWidth(to_px(property.value()));
    } else if (property.name() == "height"s) {
        layout->setHeight(to_px(property.value()));
    } else if (property.name() == "spacing"s) {
        layout->setSpacing(to_px(property.value()));
    } else if (property.name().starts_with("layout-alignment")) {
        setAlignment(layout.instance, property);
    } else if (property.name().starts_with("padding")) {
        PropertyGroupBuilder padding(layout.instance, &LayoutProperty::padding, &LayoutProperty::setPadding);
        sizeFromProperty(padding.instance, property);
    } else if (property.name().starts_with("inset")) {
        PropertyGroupBuilder inset(layout.instance, &LayoutProperty::inset, &LayoutProperty::setInset);
        sizeFromProperty(inset.instance, property);
    } else if (property.name().starts_with("margin")) {
        PropertyGroupBuilder margins(layout.instance, &LayoutProperty::margins, &LayoutProperty::setMargins);
        sizeFromProperty(margins.instance, property);
    }
}

void CssLoader::setBackgroundProperty(StyleProperty *output, const cssparser::Property &property)
{
    PropertyGroupBuilder background(output, &StyleProperty::background, &StyleProperty::setBackground);

    if (property.name() == "background"s) {
        auto value = property.value();
        if (matches_keyword(value, u"none"_s)) {
            background->setColor(Union::Color{});
            background->setImage(ImageProperty::empty());
        } else if (value.type() == cssparser::Value::Type::Color) {
            background->setColor(to_color(value));
        } else if (value.type() == cssparser::Value::Type::Url) {
            setImage(background.instance, m_stylePath, property);
        }
    }

    if (property.name() == "background-color"s) {
        background->setColor(to_color(property.value()));
    }

    if (property.name() == "background-image") {
        if (matches_keyword(property.value(), u"none"_s)) {
            background->setImage(ImageProperty::empty());
        } else {
            setImage(background.instance, m_stylePath, property);
        }
    }

    if (property.name() == "background-image-mask-color") {
        PropertyGroupBuilder image(background.instance, &BackgroundProperty::image, &BackgroundProperty::setImage);
        image->setMaskColor(to_color(property.value()));
    }
}

void CssLoader::setBorderProperty(StyleProperty *output, const cssparser::Property &property)
{
    PropertyGroupBuilder border(output, &StyleProperty::border, &StyleProperty::setBorder);

    if (property.name().ends_with("radius")) {
        PropertyGroupBuilder corners(output, &StyleProperty::corners, &StyleProperty::setCorners);

        auto setCornerRadius = [](auto &&corner, qreal radius) {
            corner->setRadius(radius);
        };

        if (property.name() == "border-radius"s) {
            if (property.values().size() == 1) {
                auto radius = to_px(property.value());
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::topLeft, &CornersProperty::setTopLeft), radius);
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::topRight, &CornersProperty::setTopRight), radius);
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::bottomLeft, &CornersProperty::setBottomLeft), radius);
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::bottomRight, &CornersProperty::setBottomRight), radius);
            } else if (property.values().size() == 4) {
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::topLeft, &CornersProperty::setTopLeft), to_px(property.value(0)));
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::topRight, &CornersProperty::setTopRight), to_px(property.value(1)));
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::bottomRight, &CornersProperty::setBottomRight),
                                to_px(property.value(2)));
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::bottomLeft, &CornersProperty::setBottomLeft),
                                to_px(property.value(3)));
            }
        } else if (property.name() == "border-top-left-radius") {
            setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::topLeft, &CornersProperty::setTopLeft), to_px(property.value()));
        } else if (property.name() == "border-top-right-radius") {
            setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::topRight, &CornersProperty::setTopRight), to_px(property.value()));
        } else if (property.name() == "border-bottom-left-radius") {
            setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersProperty::bottomLeft, &CornersProperty::setBottomLeft), to_px(property.value()));
        } else if (property.name() == "border-bottom-right-radius") {
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

    if (property.name().starts_with("text-alignment")) {
        setAlignment(text.instance, property);
    }

    if (property.name().starts_with("font")) {
        auto font = text->font().value_or(QFont{});

        if (property.name() == "font-family") {
            font.setFamily(QString::fromStdString(property.value<std::string>()));
        } else if (property.name() == "font-size") {
            auto dimension = property.value<cssparser::Dimension>();
            switch (dimension.unit()) {
            case cssparser::Dimension::Unit::Px:
                font.setPixelSize(int(dimension.value()));
                break;
            case cssparser::Dimension::Unit::Pt:
                font.setPointSizeF(dimension.value());
                break;
            case cssparser::Dimension::Unit::Percent:
                font.setPointSizeF(font.pointSizeF() * dimension.value());
                break;
            default:
                qCWarning(UNION_CSS) << "Invalid unit for font-size";
                break;
            }
        } else if (property.name() == "font-weight") {
            if (property.value().type() == cssparser::Value::Type::Integer) {
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

    if (property.name() == "color" || property.name() == "text-color") {
        text->setColor(to_color(property.value()));
    }
}

void CssLoader::setIconProperty(StyleProperty *output, const cssparser::Property &property)
{
    PropertyGroupBuilder icon(output, &StyleProperty::icon, &StyleProperty::setIcon);

    if (property.name().starts_with("icon-alignment")) {
        setAlignment(icon.instance, property);
    }

    if (property.name() == "icon-name") {
        icon->setName(QString::fromStdString(property.value<std::string>()));
    } else if (property.name() == "icon-width") {
        icon->setWidth(to_px(property.value()));
    } else if (property.name() == "icon-height") {
        icon->setHeight(to_px(property.value()));
    } else if (property.name() == "icon-size") {
        icon->setWidth(to_px(property.value()));
        icon->setHeight(to_px(property.value()));
    } else if (property.name() == "color" || property.name() == "icon-color") {
        icon->setColor(to_color(property.value()));
    }
}

void CssLoader::setShadowProperty(StyleProperty *output, const cssparser::Property &property)
{
    if (property.name() == "box-shadow") {
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
