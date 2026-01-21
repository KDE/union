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

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription(u"A tool to inspect Union's style rules"_s);
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({
        // {u"plugin"_s, u"The input plugin to use"_s},
        {u"style"_s, u"The style to use"_s, u"style"_s},
        {u"type"_s, u"The type to match"_s, u"type"_s},
        {u"id"_s, u"The ID to match"_s, u"id"_s},
        {u"states"_s, u"The states to match"_s, u"states"_s},
        {u"colorset"_s, u"The color set to match"_s, u"colorset"_s},
        {u"hint"_s, u"The hint to match. Can be specified multiple times."_s, u"hint"_s},
    });

    parser.process(app);

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

    Union::Element::Ptr queryElement = Union::Element::create();
    queryElement->setType(parser.value(u"type"_s));
    queryElement->setId(parser.value(u"id"_s));

    if (parser.isSet(u"states"_s)) {
        auto statesEnum = queryElement->metaObject()->enumerator(queryElement->metaObject()->indexOfEnumerator("State"));
        queryElement->setStates(static_cast<Union::Element::States>(statesEnum.keysToValue(parser.value(u"states"_s).toUtf8().data())));
    }

    if (parser.isSet(u"colorset"_s)) {
        auto colorSetEnum = queryElement->metaObject()->enumerator(queryElement->metaObject()->indexOfEnumerator("ColorSet"));
        queryElement->setColorSet(static_cast<Union::Element::ColorSet>(colorSetEnum.keyToValue(parser.value(u"colorset"_s).toUtf8().data())));
    }

    if (parser.isSet(u"hint"_s)) {
        const auto hints = parser.values(u"hint"_s);
        queryElement->setHints(QStringList(hints.begin(), hints.end()));
    }

    query.setElements({queryElement});
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
