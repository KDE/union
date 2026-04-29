// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <iostream>

#include <QCommandLineParser>
#include <QGuiApplication>
#include <QMetaEnum>
#include <QMetaObject>

#include <Element.h>
#include <ElementQuery.h>
#include <Style.h>
#include <StyleRegistry.h>
// #include <properties/Formatter.h>

using namespace Qt::StringLiterals;

struct ArgumentsData {
    const QStringList types;
    const QStringList ids;
    const QStringList states;
    const QStringList hints;
    const QStringList attributes;

    QStringList::const_iterator currentType = types.begin();
    QStringList::const_iterator currentId = ids.begin();
    QStringList::const_iterator currentState = states.begin();
    QStringList::const_iterator currentHint = hints.begin();
    QStringList::const_iterator currentAttribute = attributes.begin();
};

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription(u"A tool to inspect Union's style rules"_s);
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({
        {u"style"_s, u"The style to use. This needs to be specified before any other arguments."_s, u"style"_s},
        {u"verbose"_s, u"Verbose logging, including full query matching logs."_s, u"verbose"_s},
        {u"type"_s, u"The type to match"_s, u"type"_s},
        {u"id"_s, u"The ID to match"_s, u"id"_s},
        {u"state"_s, u"The state to match. Can be specified multiple times per element."_s, u"state"_s},
        {u"hint"_s, u"The hint to match. Can be specified multiple times per element."_s, u"hint"_s},
        {u"attribute"_s, u"An attribute and value to match, specified as \"key=value\". Can be specified multiple times per element."_s, u"attribute"_s},
        {u"child"_s, u"Indicates that the following arguments apply to a new element."_s},
    });

    parser.process(app.arguments());

    if (parser.isSet(u"verbose"_s)) {
        QLoggingCategory::setFilterRules(u"union.query*=true"_s);
    }

    auto registry = Union::StyleRegistry::instance();
    registry->load();

    Union::Style::Ptr style;
    if (parser.isSet(u"style"_s)) {
        style = registry->style(parser.value(u"style"_s));
    } else {
        style = registry->defaultStyle();
    }

    if (!style) {
        std::cout << "No usable style found!\n";
        return 1;
    }

    Union::ElementQuery query(style);

    Union::ElementList elements;
    Union::Element::Ptr currentElement = Union::Element::create();
    Union::Element::States currentStates;
    QStringList currentHints;
    QVariantMap currentAttributes;

    auto appendElement = [&]() {
        currentElement->setStates(currentStates);
        currentElement->setHints(currentHints);
        currentElement->setAttributes(currentAttributes);
        elements.append(currentElement);

        currentElement = Union::Element::create();
        currentStates = Union::Element::States{};
        currentHints = QStringList{};
        currentAttributes = QVariantMap{};
    };

    ArgumentsData data{
        .types = parser.values(u"type"_s),
        .ids = parser.values(u"id"_s),
        .states = parser.values(u"state"_s),
        .hints = parser.values(u"hint"_s),
        .attributes = parser.values(u"attribute"_s),
    };

    for (auto argument : parser.optionNames()) {
        if (argument == u"type") {
            currentElement->setType(*data.currentType++);
        } else if (argument == u"id") {
            currentElement->setId(*data.currentId++);
        } else if (argument == u"state") {
            const auto statesEnum = QMetaEnum::fromType<Union::Element::States>();
            auto value = static_cast<Union::Element::State>(statesEnum.keyToValue((*data.currentState++).toUtf8().data()));
            currentStates &= value;
        } else if (argument == u"hint") {
            currentHints.append(*data.currentHint++);
        } else if (argument == u"attribute") {
            auto value = *data.currentAttribute++;
            auto parts = value.split(u'=');
            if (parts.length() != 2) {
                std::cout << "Ignoring invalid attribute" << value.toStdString();
            } else {
                currentAttributes.insert(parts.at(0), parts.at(1));
            }
        } else if (argument == u"child") {
            appendElement();
        }
    }
    appendElement();

    query.setElements(elements);
    query.execute();

    if (!query.hasMatches() || !query.properties()) {
        std::cout << "Query had no result!\n";
        return 2;
    }

    std::cout << "Matched properties:\n";
    std::cout << std::endl;

    std::cout << qPrintable(query.properties()->toString(0, Union::Properties::ToStringFlag::MultiLine));

    return 0;
}
