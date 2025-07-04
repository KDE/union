// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <QtTest>

#include <Element.h>
#include <Selector.h>

using namespace Union;
using namespace Qt::StringLiterals;

class TestSelector : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEmptySelectors()
    {
        auto emptyElement = Element::create();

        Selector emptySelector = Selector::create();
        QVERIFY2(!emptySelector.matches(emptyElement), "Empty selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::Type>(QString{});
        QVERIFY2(!emptySelector.matches(emptyElement), "Empty Type selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::Id>(QString{});
        QVERIFY(!emptySelector.matches(emptyElement));
        QVERIFY2(!emptySelector.matches(emptyElement), "Empty ID selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::State>(Element::State::None);
        QVERIFY2(!emptySelector.matches(emptyElement), "Empty State selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::Hint>(QString{});
        QVERIFY2(!emptySelector.matches(emptyElement), "Empty Hint selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::Attribute>(std::make_pair(QString{}, QVariant{}));
        QVERIFY2(!emptySelector.matches(emptyElement), "Empty Attribute selector should not match an empty element");
    }

    void testSimpleStructureMatches_data()
    {
        QTest::addColumn<SelectorList>("selectors");
        QTest::addColumn<bool>("expected");
        QTest::addColumn<int>("weight");

        SelectorList selectors = {
            Selector::create<SelectorType::Id>(u"id"_s),
            Selector::create<SelectorType::Type>(u"Type"_s),
            Selector::create<SelectorType::State>(Element::State::Hovered),
        };
        QTest::addRow("exact") << selectors << true << 111;

        selectors = {
            Selector::create<SelectorType::Type>(u"Type"_s),
            Selector::create<SelectorType::State>(Element::State::Hovered),
        };
        QTest::addRow("skip first") << selectors << true << 11;

        selectors = {
            Selector::create<SelectorType::Id>(u"id"_s),
            Selector::create<SelectorType::State>(Element::State::Hovered),
        };
        QTest::addRow("skip middle") << selectors << true << 110;

        selectors = {
            Selector::create<SelectorType::Id>(u"id"_s),
            Selector::create<SelectorType::Type>(u"Type"_s),
        };
        QTest::addRow("skip last") << selectors << false << 101;

        selectors = {
            Selector::create<SelectorType::Id>(u"id"_s),
        };
        QTest::addRow("id only") << selectors << false << 100;

        selectors = {
            Selector::create<SelectorType::Type>(u"Type"_s),
        };
        QTest::addRow("type only") << selectors << false << 1;

        selectors = {
            Selector::create<SelectorType::State>(Element::State::Hovered),
        };
        QTest::addRow("state only") << selectors << true << 10;

        selectors = {
            Selector::create<SelectorType::AnyElement>(),
        };
        QTest::addRow("any element") << selectors << true << -100;
    }

    void testSimpleStructureMatches()
    {
        ElementList elements;
        auto element = Element::create();
        element->setId(u"id"_s);
        elements.append(element);

        element = Element::create();
        element->setType(u"Type"_s);
        elements.append(element);

        element = Element::create();
        element->setStates(Element::State::Hovered);
        elements.append(element);

        QFETCH(SelectorList, selectors);
        QFETCH(bool, expected);
        QFETCH(int, weight);

        QCOMPARE(selectors.matches(elements), expected);
        QCOMPARE(selectors.weight(), weight);
    }

    void testSkipEmpty()
    {
        ElementList elements;
        auto element = Element::create();
        elements.append(element);

        element = Element::create();
        element->setType(u"type"_s);
        elements.append(element);

        element = Element::create();
        elements.append(element);

        element = Element::create();
        element->setStates(Element::State::Hovered);
        elements.append(element);

        SelectorList selectors = {
            Selector::create<SelectorType::Type>(u"type"_s),
            Selector::create<SelectorType::State>(Element::State::Hovered),
        };

        QVERIFY(selectors.matches(elements));
    }
};

QTEST_MAIN(TestSelector)

#include "TestSelector.moc"
