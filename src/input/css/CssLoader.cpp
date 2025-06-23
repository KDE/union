// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "CssLoader.h"

#include <source_location>

#include <QFile>
#include <QMetaEnum>
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

template<typename T>
inline int toEnumIntValue(const std::string &value)
{
    const auto metaEnum = QMetaEnum::fromType<T>();

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
    styleSheet.parse_file("generated-properties.css"s);
    styleSheet.parse_file("extra-properties.css"s);
    styleSheet.parse_file("default.css"s);

    auto stylePath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"union/css/styles/breeze"_s, QStandardPaths::LocateDirectory);

    styleSheet.set_root_path(fs::path(stylePath.toStdString()));
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
        } else if (property.name.starts_with("text")) {
            setTextProperty(result, property);
        } else if (property.name.starts_with("icon")) {
            setIconProperty(result, property);
        } else if (property.name == "color"s) {
            setTextProperty(result, property);
        } else if (property.name.starts_with("shadow")) {
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
        layout.setInset(sizeFromProperty(layout.padding(), property));
    } else if (property.name.starts_with("margin")) {
        layout.setMargins(sizeFromProperty(layout.margins(), property));
    }

    if (layout.hasAnyValue()) {
        output.setLayout(layout);
    }
}

void CssLoader::setBackgroundProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto background = output.background().value_or(BackgroundProperty{});

    if (property.name == "background"s || property.name == "background-color"s) {
        background.setColor(to_qcolor(property.value()));
    }

    if (background.hasAnyValue()) {
        output.setBackground(background);
    }
}

void CssLoader::setBorderProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto border = output.border().value_or(BorderProperty{});
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
            corners.setBottomLeft(setCornerRadius(corners.bottomLeft(), to_px(property.value(2))));
            corners.setBottomRight(setCornerRadius(corners.bottomRight(), to_px(property.value(3))));
        }
    }

    if (property.name == "border"s) {
        LineProperty line;
        line.setSize(to_px(property.value(0)));
        line.setColor(to_qcolor(property.value(2)));

        border.setLeft(line);
        border.setRight(line);
        border.setTop(line);
        border.setBottom(line);
    }

    if (border.hasAnyValue()) {
        output.setBorder(border);
    }
    if (corners.hasAnyValue()) {
        output.setCorners(corners);
    }
}

void CssLoader::setTextProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto text = output.text().value_or(TextProperty{});

    if (property.name.starts_with("text-alignment")) {
        setAlignment(text, property);
    }

    if (text.hasAnyValue()) {
        output.setText(text);
    }
}

void CssLoader::setIconProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto icon = output.icon().value_or(IconProperty{});

    if (property.name.starts_with("icon-alignment")) {
        setAlignment(icon, property);
    }

    if (icon.hasAnyValue()) {
        output.setIcon(icon);
    }
}

void CssLoader::setShadowProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto shadow = output.shadow().value_or(ShadowProperty{});

    if (property.name == "shadow") {
        shadow.setSize(to_px(property.value(0)));
        shadow.setColor(to_qcolor(property.value(3)));

        auto offset = shadow.offset().value_or(OffsetProperty{});
        offset.setHorizontal(to_px(property.value(1)));
        offset.setVertical(to_px(property.value(2)));
    }

    if (shadow.hasAnyValue()) {
        output.setShadow(shadow);
    }
}
