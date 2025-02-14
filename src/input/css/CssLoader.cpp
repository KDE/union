// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "CssLoader.h"

#include <QFile>
#include <QMetaEnum>

#include <StyleRule.h>
#include <Theme.h>

#include <CssParser.h>

#include "css_logging.h"

using namespace Union;
using namespace Union::Properties;
using namespace Qt::StringLiterals;
using namespace std::string_literals;

bool CssLoader::load(Theme::Ptr theme)
{
    // TODO: Proper style discovery
    QFile cssFile(u"test.css"_s);
    if (!cssFile.open(QIODevice::ReadOnly)) {
        qCWarning(UNION_CSS) << "Could not open test.css";
        return false;
    }

    auto data = cssFile.readAll();

    cssparser::CssParser parser;
    const auto result = parser.parse(data.toStdString());

    for (const auto &rule : result) {
        if (rule.properties.empty()) {
            continue;
        }

        for (const auto &selector : rule.selectors) {
            auto styleRule = StyleRule::create();
            styleRule->setSelectors(createSelectorList(selector));
            styleRule->setProperties(createProperties(rule.properties));
            theme->insert(styleRule);
        }
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

Union::Properties::StyleProperty CssLoader::createProperties(const std::vector<cssparser::Property> &properties)
{
    Union::Properties::StyleProperty result;

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
    }

    return result;
}

void CssLoader::setLayoutProperty(Union::Properties::StyleProperty &output, const cssparser::Property &property)
{
    auto layout = output.layout().value_or(Union::Properties::LayoutProperty{});

    if (property.name == "width"s) {
        layout.setWidth(std::get<float>(property.values.at(0)));
    }

    if (property.name == "height"s) {
        layout.setHeight(std::get<float>(property.values.at(0)));
    }

    output.setLayout(layout);
}

void CssLoader::setBackgroundProperty(Union::Properties::StyleProperty &output, const cssparser::Property &property)
{
    auto background = output.background().value_or(Union::Properties::BackgroundProperty{});

    if (property.name == "background"s || property.name == "background-color"s) {
        auto color = std::get<cssparser::Color>(property.values.at(0));
        background.setColor(QColor{color.r, color.g, color.b, color.a});
    }

    if (background.hasAnyValue()) {
        output.setBackground(background);
    }
}

void CssLoader::setBorderProperty(Union::Properties::StyleProperty &output, const cssparser::Property &property)
{
    auto background = output.background().value_or(Union::Properties::BackgroundProperty{});

    auto border = background.border().value_or(Union::Properties::BorderProperty{});
    auto corners = background.corners().value_or(Union::Properties::CornersProperty{});

    if (property.name == "border"s) {
        auto width = std::get<float>(property.values.at(0));
        // auto style = std::get<std::string>(property.values.at(1));
        auto color = std::get<cssparser::Color>(property.values.at(2));

        Union::Properties::LineProperty line;
        line.setSize(width);
        line.setColor(QColor{color.r, color.g, color.b, color.a});

        border.setLeft(line);
        border.setRight(line);
        border.setTop(line);
        border.setBottom(line);

        Union::Properties::CornerProperty corner;
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
