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

template<typename F>
inline F to_enum_value(const std::string &value)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<F>();

    auto count = metaEnum.keyCount();
    for (int i = 0; i < count; ++i) {
        if (QByteArray(metaEnum.key(i)).toLower().toStdString() == value) {
            return F{metaEnum.value(i)};
        }
    }

    return F{};
}

template<typename T>
inline void setAlignment(T &output, const cssparser::Property &property)
{
    auto alignment = output.alignment().value_or(AlignmentProperty{});

    if (property.name.ends_with("alignment-container"s)) {
        auto value = to_enum_value<AlignmentContainer>(std::get<std::string>(property.values.at(0)));
        alignment.setContainer(value);
    } else if (property.name.ends_with("alignment-horizontal"s)) {
        auto value = to_enum_value<Alignment>(std::get<std::string>(property.values.at(0)));
        alignment.setHorizontal(value);
    } else if (property.name.ends_with("alignment-vertical"s)) {
        auto value = to_enum_value<Alignment>(std::get<std::string>(property.values.at(0)));
        alignment.setVertical(value);
    } else if (property.name.ends_with("alignment-order"s)) {
        auto value = std::get<int>(property.values.at(0));
        alignment.setOrder(value);
    }

    if (alignment.hasAnyValue()) {
        output.setAlignment(alignment);
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
        qDebug() << styleRule << styleRule->properties();
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
    case cssparser::SelectorKind::Type:
        return Union::Selector::create<Union::SelectorType::Type>(QString::fromStdString(std::get<std::string>(part.value)));
    case cssparser::SelectorKind::Id:
        return Union::Selector::create<Union::SelectorType::Id>(QString::fromStdString(std::get<std::string>(part.value)));
    case cssparser::SelectorKind::PseudoClass: {
        auto metaEnum = QMetaEnum::fromType<Union::Element::States>();
        auto value = std::get<std::string>(part.value);
        auto state = Union::Element::State(metaEnum.keyToValue(value.data()));
        return Union::Selector::create<Union::SelectorType::State>(state);
    }
    case cssparser::SelectorKind::Class:
        return Union::Selector::create<Union::SelectorType::Hint>(QString::fromStdString(std::get<std::string>(part.value)));
    case cssparser::SelectorKind::Attribute:
        // TODO
        break;
    default:
        break;
    }

    return Union::Selector::create();
}

StyleProperty CssLoader::createProperties(const std::vector<cssparser::Property> &properties)
{
    StyleProperty result;

    for (auto property : properties) {
        if (property.name == "width"s || property.name == "height"s) {
            setLayoutProperty(result, property);
        }

        if (property.name.starts_with("layout")) {
            setLayoutProperty(result, property);
        }

        if (property.name.starts_with("background")) {
            setBackgroundProperty(result, property);
        }

        if (property.name.starts_with("border")) {
            setBorderProperty(result, property);
        }

        if (property.name.starts_with("text")) {
            setTextProperty(result, property);
        }
    }

    return result;
}

void CssLoader::setLayoutProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto layout = output.layout().value_or(LayoutProperty{});

    if (property.name == "width"s) {
        auto length = std::get<cssparser::Dimension>(property.values.at(0));
        layout.setWidth(length.value);
    } else if (property.name == "height"s) {
        auto length = std::get<cssparser::Dimension>(property.values.at(0));
        layout.setHeight(length.value);
    } else if (property.name.starts_with("layout-alignment")) {
        setAlignment(layout, property);
    }

    if (layout.hasAnyValue()) {
        output.setLayout(layout);
    }
}

void CssLoader::setBackgroundProperty(StyleProperty &output, const cssparser::Property &property)
{
    auto background = output.background().value_or(BackgroundProperty{});

    if (property.name == "background"s || property.name == "background-color"s) {
        auto color = std::get<cssparser::Color>(property.values.at(0));
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
