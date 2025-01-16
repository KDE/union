// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <iostream>

#include <QCommandLineParser>
#include <QGuiApplication>
#include <QMetaEnum>
#include <QMetaObject>

#include <Element.h>
#include <ElementQuery.h>
#include <Theme.h>
#include <ThemeRegistry.h>
#include <properties/Formatter.h>

using namespace Qt::StringLiterals;

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription(u"A tool to inspect Union's theme rules"_s);
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({
        // {u"plugin"_s, u"The input plugin to use"_s},
        {u"theme"_s, u"The theme to use"_s, u"theme"_s},
        {u"type"_s, u"The type to match"_s, u"type"_s},
        {u"id"_s, u"The ID to match"_s, u"id"_s},
        {u"states"_s, u"The states to match"_s, u"states"_s},
        {u"colorset"_s, u"The color set to match"_s, u"colorset"_s},
    });

    parser.process(app);

    auto registry = Union::ThemeRegistry::instance();
    registry->load();

    Union::Theme::Ptr theme;
    if (parser.isSet(u"theme"_s)) {
        theme = registry->theme(parser.value(u"theme"_s));
    } else {
        theme = registry->defaultTheme();
    }

    if (!theme) {
        std::cout << "No usable theme found!\n";
        exit(1);
    }

    Union::ElementQuery query(theme);

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

    query.setElements({queryElement});
    query.execute();

    if (!query.hasMatches()) {
        std::cout << "Query had no result!\n";
        exit(2);
    }

    std::cout << "Matched properties:\n";
    std::cout << std::endl;

    std::cout << std::format("{:nl}", query.properties());

    exit(0);
}
