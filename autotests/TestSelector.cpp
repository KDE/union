/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include <QtTest>

#include <Element.h>
#include <Selector.h>

using namespace Union;

class TestSelector : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testSimpleStructureMatches_data()
    {
        QTest::addColumn<SelectorList>("selectors");
        QTest::addColumn<bool>("expected");

        SelectorList selectors = {
            Selector(Selector::SelectorType::Id, u"id"_qs),
            Selector(Selector::SelectorType::Type, u"Type"_qs),
            Selector(Selector::SelectorType::State, u"state"_qs),
        };
        QTest::addRow("exact") << selectors << true;

        selectors = {
            Selector(Selector::SelectorType::Type, u"Type"_qs),
            Selector(Selector::SelectorType::State, u"state"_qs),
        };
        QTest::addRow("skip first") << selectors << true;

        selectors = {
            Selector(Selector::SelectorType::Id, u"id"_qs),
            Selector(Selector::SelectorType::State, u"state"_qs),
        };
        QTest::addRow("skip middle") << selectors << true;

        selectors = {
            Selector(Selector::SelectorType::Id, u"id"_qs),
            Selector(Selector::SelectorType::Type, u"Type"_qs),
        };
        QTest::addRow("skip last") << selectors << false;

        selectors = {
            Selector(Selector::SelectorType::Id, u"id"_qs),
        };
        QTest::addRow("id only") << selectors << false;

        selectors = {
            Selector(Selector::SelectorType::Type, u"Type"_qs),
        };
        QTest::addRow("type only") << selectors << false;

        selectors = {
            Selector(Selector::SelectorType::State, u"state"_qs),
        };
        QTest::addRow("state only") << selectors << true;
    }

    void testSimpleStructureMatches()
    {
        ElementList elements;
        auto element = Element::create();
        element->setId(u"id"_qs);
        elements.append(element);

        element = Element::create();
        element->setType(u"Type"_qs);
        elements.append(element);

        element = Element::create();
        element->setStates({u"state"_qs});
        elements.append(element);

        QFETCH(SelectorList, selectors);
        QFETCH(bool, expected);

        QCOMPARE(selectorListMatches(selectors, elements), expected);
    }

    void testSkipEmpty()
    {
        ElementList elements;
        auto element = Element::create();
        elements.append(element);

        element = Element::create();
        element->setType(u"type"_qs);
        elements.append(element);

        element = Element::create();
        elements.append(element);

        element = Element::create();
        element->setStates({u"state"_qs});
        elements.append(element);

        SelectorList selectors = {
            Selector(Selector::SelectorType::Type, u"type"_qs),
            Selector(Selector::SelectorType::State, u"state"_qs),
        };

        QVERIFY(selectorListMatches(selectors, elements));
    }
};

QTEST_MAIN(TestSelector)

#include "TestSelector.moc"
