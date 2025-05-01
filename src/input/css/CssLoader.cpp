// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "CssLoader.h"

#include <source_location>

#include <QFile>
#include <QMetaEnum>

#include <StyleRule.h>
#include <Theme.h>

#include <CssParser.h>

#include "css_logging.h"

using namespace Qt::StringLiterals;
using namespace std::string_literals;

namespace fs = std::filesystem;

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
        auto value = toEnumValue<AlignmentContainer>(std::get<std::string>(property.values.at(0)));
        alignment.setContainer(value);
    } else if (property.name.ends_with("alignment-horizontal"s)) {
        auto value = toEnumValue<Alignment>(std::get<std::string>(property.values.at(0)));
        alignment.setHorizontal(value);
    } else if (property.name.ends_with("alignment-vertical"s)) {
        auto value = toEnumValue<Alignment>(std::get<std::string>(property.values.at(0)));
        alignment.setVertical(value);
    } else if (property.name.ends_with("alignment-order"s)) {
        auto value = std::get<int>(property.values.at(0));
        alignment.setOrder(value);
    }

    if (alignment.hasAnyValue()) {
        output.setAlignment(alignment);
    }
}

inline std::optional<SizeProperty> sizeFromProperty(const std::optional<SizeProperty> &input, const cssparser::Property &property)
{
    auto result = input.value_or(SizeProperty{});

    if (property.name.ends_with("left")) {
        result.setLeft(property.value<cssparser::Dimension>().value);
    } else if (property.name.ends_with("right")) {
        result.setRight(property.value<cssparser::Dimension>().value);
    } else if (property.name.ends_with("top")) {
        result.setTop(property.value<cssparser::Dimension>().value);
    } else if (property.name.ends_with("bottom")) {
        result.setBottom(property.value<cssparser::Dimension>().value);
    } else {
        if (property.values.size() == 1) {
            auto value = property.value<cssparser::Dimension>().value;
            result.setLeft(value);
            result.setRight(value);
            result.setTop(value);
            result.setBottom(value);
        } else if (property.values.size() == 2) {
            auto horizontal = property.value<cssparser::Dimension>(0).value;
            auto vertical = property.value<cssparser::Dimension>(1).value;
            result.setLeft(horizontal);
            result.setRight(horizontal);
            result.setTop(vertical);
            result.setBottom(vertical);
        } else if (property.values.size() == 4) {
            result.setLeft(property.value<cssparser::Dimension>(0).value);
            result.setRight(property.value<cssparser::Dimension>(1).value);
            result.setTop(property.value<cssparser::Dimension>(2).value);
            result.setBottom(property.value<cssparser::Dimension>(3).value);
        }
    }

    if (result.hasAnyValue()) {
        return result;
    } else {
        return std::nullopt;
    }
}

bool CssLoader::load(Theme::Ptr theme)
{
    auto location = std::source_location::current();

    cssparser::StyleSheet styleSheet;
    styleSheet.set_root_path(fs::path(location.file_name()).parent_path());
    styleSheet.parse_file("test.css");

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
        }
    }

    return result;
}

void CssLoader::setLayoutProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto layout = output.layout().value_or(LayoutProperty{});

    qDebug() << property.name; // << property.values;

    if (property.name == "width"s) {
        layout.setWidth(property.value<cssparser::Dimension>().value);
    } else if (property.name == "height"s) {
        layout.setHeight(property.value<cssparser::Dimension>().value);
    } else if (property.name == "spacing"s) {
        layout.setSpacing(property.value<cssparser::Dimension>().value);
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
        auto color = property.value<cssparser::Color>();
        background.setColor(QColor{color.r, color.g, color.b, color.a});
    }

    if (background.hasAnyValue()) {
        output.setBackground(background);
    }
}

void CssLoader::setBorderProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto background = output.background().value_or(BackgroundProperty{});

    auto border = background.border().value_or(BorderProperty{});
    auto corners = background.corners().value_or(CornersProperty{});

    if (property.name == "border"s) {
        auto width = std::get<cssparser::Dimension>(property.values.at(0));
        // auto style = std::get<std::string>(property.values.at(1));
        auto color = std::get<cssparser::Color>(property.values.at(2));

        LineProperty line;
        line.setSize(width.value);
        line.setColor(QColor{color.r, color.g, color.b, color.a});

        border.setLeft(line);
        border.setRight(line);
        border.setTop(line);
        border.setBottom(line);

        CornerProperty corner;
        corner.setColor(QColor{color.r, color.g, color.b, color.a});
        // corner.setWidth(width);
        // corner.setHeight(width);
        corners.setTopLeft(corner);
        corners.setTopRight(corner);
        corners.setBottomLeft(corner);
        corners.setBottomRight(corner);
    }

    if (border.hasAnyValue()) {
        background.setBorder(border);
    }
    if (corners.hasAnyValue()) {
        background.setCorners(corners);
    }

    if (background.hasAnyValue()) {
        output.setBackground(background);
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
