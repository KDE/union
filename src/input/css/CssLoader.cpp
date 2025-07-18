// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "CssLoader.h"

#include <source_location>

#include <QFile>
#include <QMetaEnum>
#include <QRegularExpression>
#include <QStandardPaths>

#include <StyleRule.h>
#include <Theme.h>

#include <CssParser.h>

#include "css_logging.h"

using namespace Qt::StringLiterals;
using namespace std::string_literals;

namespace fs = std::filesystem;

float to_px(const cssparser::Value &value)
{
    if (!std::holds_alternative<cssparser::Dimension>(value)) {
        return 0.0;
    }

    auto dimension = std::get<cssparser::Dimension>(value);
    switch (dimension.unit) {
    case cssparser::Unit::Px:
        return dimension.value;
    default:
        return 0.0;
    }
}

QColor to_qcolor(const cssparser::Value &value)
{
    if (!std::holds_alternative<cssparser::Color>(value)) {
        return QColor{};
    }

    auto color = std::get<cssparser::Color>(value);
    return QColor::fromRgb(color.r, color.g, color.b, color.a);
}

fs::path to_path(const cssparser::Value &value)
{
    if (!std::holds_alternative<cssparser::Url>(value)) {
        return fs::path{};
    }

    auto url = std::get<cssparser::Url>(value);
    return url.data;
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
inline void setAlignment(T &output, const cssparser::Property &property)
{
    auto alignment = output.alignment().value_or(AlignmentProperty{});

    if (property.name.ends_with("alignment-container"s)) {
        alignment.setContainer(toEnumValue<AlignmentContainer>(property.value<std::string>()));
    } else if (property.name.ends_with("alignment-horizontal"s)) {
        alignment.setHorizontal(toEnumValue<Alignment>(property.value<std::string>()));
    } else if (property.name.ends_with("alignment-vertical"s)) {
        alignment.setVertical(toEnumValue<Alignment>(property.value<std::string>()));
    } else if (property.name.ends_with("alignment-order"s)) {
        alignment.setOrder(property.value<int>(0));
    } else if (property.values.size() == 4) {
        alignment.setContainer(toEnumValue<AlignmentContainer>(property.value<std::string>(0)));
        alignment.setHorizontal(toEnumValue<Alignment>(property.value<std::string>(1)));
        alignment.setVertical(toEnumValue<Alignment>(property.value<std::string>(2)));
        alignment.setOrder(property.value<int>(3));
    }

    if (alignment.hasAnyValue()) {
        output.setAlignment(alignment);
    }
}

inline std::optional<SizeProperty> sizeFromProperty(const std::optional<SizeProperty> &input, const cssparser::Property &property)
{
    auto result = input.value_or(SizeProperty{});

    if (property.name.ends_with("left")) {
        result.setLeft(to_px(property.value()));
    } else if (property.name.ends_with("right")) {
        result.setRight(to_px(property.value()));
    } else if (property.name.ends_with("top")) {
        result.setTop(to_px(property.value()));
    } else if (property.name.ends_with("bottom")) {
        result.setBottom(to_px(property.value()));
    } else {
        if (property.values.size() == 1) {
            auto value = to_px(property.value());
            result.setLeft(value);
            result.setRight(value);
            result.setTop(value);
            result.setBottom(value);
        } else if (property.values.size() == 2) {
            auto horizontal = to_px(property.value(0));
            auto vertical = to_px(property.value(1));
            result.setLeft(horizontal);
            result.setRight(horizontal);
            result.setTop(vertical);
            result.setBottom(vertical);
        } else if (property.values.size() == 3) {
            result.setTop(to_px(property.value(0)));
            result.setRight(to_px(property.value(1)));
            result.setBottom(to_px(property.value(2)));
            result.setLeft(to_px(property.value(1)));
        } else if (property.values.size() == 4) {
            result.setTop(to_px(property.value(0)));
            result.setRight(to_px(property.value(1)));
            result.setBottom(to_px(property.value(2)));
            result.setLeft(to_px(property.value(3)));
        }
    }

    if (result.hasAnyValue()) {
        return result;
    } else {
        return std::nullopt;
    }
}

template<typename T>
void setDirectionValue(T &output, const std::string &baseName, const cssparser::Property &property)
{
    QList<QByteArray> directions;
    QList<QByteArray> properties;
    QList<cssparser::Value> values;

    if (property.name == baseName) {
        directions = {"left", "right", "top", "bottom"};
        properties = {"width", "color"};
        values = {property.value(0), property.value(2)};
    } else if (property.name == baseName + "-width") {
        directions = {"left", "right", "top", "bottom"};
        properties = {"width"};
        values = {property.value()};
    } else if (property.name == baseName + "-color") {
        directions = {"left", "right", "top", "bottom"};
        properties = {"color"};
        values = {property.value()};
    } else {
        static const QRegularExpression directionValueExpression{QStringLiteral(R"(([^-]+)-([^-]+)(?:-(.+))?)"),
                                                                 QRegularExpression::PatternOption::CaseInsensitiveOption};
        auto matches = directionValueExpression.match(QString::fromStdString(property.name));

        if (!matches.hasMatch() || matches.captured(1).toStdString() != baseName) {
            return;
        }

        directions = {matches.captured(2).toLatin1()};

        if (matches.hasCaptured(3)) {
            properties = {matches.captured(3).toLatin1()};
            values = {property.value()};
        } else {
            properties = {"width", "color"};
            values = {property.value(0), property.value(2)};
        }
    }

    auto setLineValue = [](LineProperty &line, const QByteArray &property, const cssparser::Value &value) -> LineProperty {
        if (property == "width") {
            line.setSize(to_px(value));
        } else if (property == "size") {
            line.setSize(to_px(value));
        } else if (property == "color") {
            line.setColor(to_qcolor(value));
        }
        return line;
    };

    for (const auto &direction : std::as_const(directions)) {
        for (qsizetype i = 0; i < properties.size(); ++i) {
            auto property = properties.at(i);
            auto value = values.at(i);
            LineProperty line;
            if (direction == "left") {
                auto line = output.left_or_new();
                output.setLeft(setLineValue(line, property, value));
            } else if (direction == "right") {
                auto line = output.right_or_new();
                output.setRight(setLineValue(line, property, value));
            } else if (direction == "top") {
                auto line = output.top_or_new();
                output.setTop(setLineValue(line, property, value));
            } else if (direction == "bottom") {
                auto line = output.left_or_new();
                output.setBottom(setLineValue(line, property, value));
            }
        }
    }
}

inline CornerProperty setCornerRadius(const std::optional<CornerProperty> &corner, qreal radius)
{
    auto property = corner.value_or(CornerProperty{});
    property.setRadius(radius);
    return property;
}

bool CssLoader::load(Theme::Ptr theme)
{
    auto defaultsPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"union/css/defaults"_s, QStandardPaths::LocateDirectory);

    // This is a bit hacky but allows us to provide defaults that the actual
    // style doesn't need to care about. Ultimately we want to do something
    // cleaner here.
    cssparser::StyleSheet styleSheet;
    styleSheet.set_root_path(fs::path(defaultsPath.toStdString()));
    styleSheet.parse_file("default.css"s);

    m_stylePath =
        fs::path(QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"union/css/styles/breeze"_s, QStandardPaths::LocateDirectory).toStdString());

    styleSheet.set_root_path(m_stylePath);
    styleSheet.parse_file("style.css");

    for (const auto &rule : styleSheet.rules()) {
        if (rule.properties.empty()) {
            continue;
        }

        auto styleRule = StyleRule::create();
        styleRule->setSelectors(createSelectorList(rule.selector));
        styleRule->setProperties(createProperties(rule.properties));
        theme->insert(styleRule);
    }

    return true;
}

Union::SelectorList CssLoader::createSelectorList(const cssparser::Selector &selector)
{
    Union::SelectorList result;

    auto itr = selector.parts.begin();
    while (itr != selector.parts.end()) {
        Union::SelectorList all;

        while (itr != selector.parts.end() && !itr->is_combinator()) {
            all.push_back(createSelector(*itr++));
        }

        if (!all.isEmpty()) {
            result.appendAllOf(all);
        }

        if (itr != selector.parts.end()) {
            itr++;
        }
    }

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
    case cssparser::SelectorKind::Attribute:
        // TODO
        break;
    case cssparser::SelectorKind::DocumentRoot:
        // TODO
        break;
    case cssparser::SelectorKind::DescendantCombinator:
        // TODO
        break;
    case cssparser::SelectorKind::ChildCombinator:
        // TODO
        break;
    }

    return Union::Selector::create();
}

StyleProperty CssLoader::createProperties(const std::vector<cssparser::Property> &properties)
{
    StyleProperty result;

    for (auto property : properties) {
        if (property.name == "width"s || property.name == "height"s || property.name == "spacing"s) {
            setLayoutProperty(result, property);
        } else if (property.name.starts_with("padding")) {
            setLayoutProperty(result, property);
        } else if (property.name.starts_with("inset")) {
            setLayoutProperty(result, property);
        } else if (property.name.starts_with("margin")) {
            setLayoutProperty(result, property);
        } else if (property.name.starts_with("layout")) {
            setLayoutProperty(result, property);
        } else if (property.name.starts_with("background")) {
            setBackgroundProperty(result, property);
        } else if (property.name.starts_with("border")) {
            setBorderProperty(result, property);
        } else if (property.name.starts_with("outline")) {
            setOutlineProperty(result, property);
        } else if (property.name.starts_with("text") || property.name.starts_with("font")) {
            setTextProperty(result, property);
        } else if (property.name.starts_with("icon")) {
            setIconProperty(result, property);
        } else if (property.name == "color"s) {
            setTextProperty(result, property);
        } else if (property.name.starts_with("shadow") || property.name.starts_with("box-shadow")) {
            setShadowProperty(result, property);
        }
    }

    return result;
}

void CssLoader::setLayoutProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto layout = output.layout().value_or(LayoutProperty{});

    if (property.name == "width"s) {
        layout.setWidth(to_px(property.value()));
    } else if (property.name == "height"s) {
        layout.setHeight(to_px(property.value()));
    } else if (property.name == "spacing"s) {
        layout.setSpacing(to_px(property.value()));
    } else if (property.name.starts_with("layout-alignment")) {
        setAlignment(layout, property);
    } else if (property.name.starts_with("padding")) {
        layout.setPadding(sizeFromProperty(layout.padding(), property));
    } else if (property.name.starts_with("inset")) {
        layout.setInset(sizeFromProperty(layout.inset(), property));
    } else if (property.name.starts_with("margin")) {
        layout.setMargins(sizeFromProperty(layout.margins(), property));
    }

    if (layout.hasAnyValue()) {
        output.setLayout(layout);
    }
}

void CssLoader::setBackgroundProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto background = output.background_or_new();

    if (property.name == "background-color"s) {
        background.setColor(to_qcolor(property.value()));
    }

    if (property.name == "background-image") {
        auto path = m_stylePath / to_path(property.value());

        QImage imageData;
        if (!imageData.load(QString::fromStdString(path))) {
            qCWarning(UNION_CSS) << "Could not load image" << path.string();
        }

        auto image = background.image_or_new();
        image.setImageData(imageData);
        image.setWidth(imageData.width());
        image.setHeight(imageData.height());
        background.setImage(image);
    }

    if (background.hasAnyValue()) {
        output.setBackground(background);
    }
}

void CssLoader::setBorderProperty(StyleProperty &output, const cssparser::Property &property)
{
    if (property.name.ends_with("radius")) {
        auto corners = output.corners().value_or(CornersProperty{});

        if (property.name == "border-radius"s) {
            if (property.values.size() == 1) {
                auto radius = to_px(property.value());
                corners.setTopLeft(setCornerRadius(corners.topLeft(), radius));
                corners.setTopRight(setCornerRadius(corners.topRight(), radius));
                corners.setBottomLeft(setCornerRadius(corners.bottomLeft(), radius));
                corners.setBottomRight(setCornerRadius(corners.bottomRight(), radius));
            } else if (property.values.size() == 4) {
                corners.setTopLeft(setCornerRadius(corners.topLeft(), to_px(property.value(0))));
                corners.setTopRight(setCornerRadius(corners.topRight(), to_px(property.value(1))));
                corners.setBottomRight(setCornerRadius(corners.bottomRight(), to_px(property.value(2))));
                corners.setBottomLeft(setCornerRadius(corners.bottomLeft(), to_px(property.value(3))));
            }
        } else if (property.name == "border-top-left-radius") {
            corners.setTopLeft(setCornerRadius(corners.topLeft(), to_px(property.value())));
        } else if (property.name == "border-top-right-radius") {
            corners.setTopRight(setCornerRadius(corners.topRight(), to_px(property.value())));
        } else if (property.name == "border-bottom-left-radius") {
            corners.setBottomLeft(setCornerRadius(corners.bottomLeft(), to_px(property.value())));
        } else if (property.name == "border-bottom-right-radius") {
            corners.setBottomRight(setCornerRadius(corners.bottomRight(), to_px(property.value())));
        }

        if (corners.hasAnyValue()) {
            output.setCorners(corners);
        }

        return;
    }

    auto border = output.border_or_new();
    setDirectionValue(border, "border"s, property);

    if (border.hasAnyValue()) {
        output.setBorder(border);
    }
}

void CssLoader::setOutlineProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto outline = output.outline_or_new();

    setDirectionValue(outline, "outline"s, property);

    if (outline.hasAnyValue()) {
        output.setOutline(outline);
    }
}

void CssLoader::setTextProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto text = output.text_or_new();

    if (property.name.starts_with("text-alignment")) {
        setAlignment(text, property);
    }

    if (property.name.starts_with("font")) {
        auto font = text.font().value_or(QFont{});

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

        text.setFont(font);
    }

    if (text.hasAnyValue()) {
        output.setText(text);
    }
}

void CssLoader::setIconProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto icon = output.icon_or_new();

    if (property.name.starts_with("icon-alignment")) {
        setAlignment(icon, property);
    }

    if (property.name == "icon-name") {
        icon.setName(QString::fromStdString(property.value<std::string>()));
    } else if (property.name == "icon-width") {
        icon.setWidth(to_px(property.value()));
    } else if (property.name == "icon-height") {
        icon.setHeight(to_px(property.value()));
    }

    if (icon.hasAnyValue()) {
        output.setIcon(icon);
    }
}

void CssLoader::setShadowProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto shadow = output.shadow_or_new();

    if (property.name == "box-shadow") {
        auto offset = shadow.offset_or_new();
        offset.setHorizontal(to_px(property.value(0)));
        offset.setVertical(to_px(property.value(1)));
        shadow.setOffset(offset);

        shadow.setBlur(to_px(property.value(2)));
        shadow.setSize(to_px(property.value(3)));
        shadow.setColor(to_qcolor(property.value(4)));
    }

    if (shadow.hasAnyValue()) {
        output.setShadow(shadow);
    }
}
