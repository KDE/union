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
    void testSelectorListMatches_data()
    {
        QTest::addColumn<SelectorList>("selectors");
        QTest::addColumn<ElementList>("elements");
        QTest::addColumn<bool>("expected");

        ElementList elements;
        auto element = Element::create();
        element->setId(u"window-contents"_qs);
        elements.append(element);

        element = Element::create();
        element->setType(u"Button"_qs);
        elements.append(element);

        element = Element::create();
        element->setType(u"Background"_qs);
        elements.append(element);

        SelectorList selectors;
        selectors.append(Selector(Selector::SelectorType::Type, u"Button"_qs));
        selectors.append(Selector(Selector::SelectorType::Type, u"Background"_qs));

        QTest::addRow("0") << selectors << elements << true;

        selectors.clear();
        selectors.append(Selector(Selector::SelectorType::Id, u"window-contents"_qs));
        selectors.append(Selector(Selector::SelectorType::Type, u"Background"_qs));

        QTest::addRow("1") << selectors << elements << true;

        selectors.clear();
        selectors.append(Selector(Selector::SelectorType::Id, u"window-contents"_qs));

        QTest::addRow("2") << selectors << elements << false;
    }

    void testSelectorListMatches()
    {
        // SelectorList selectors;
        // selectors.append(Selector(Selector::SelectorType::Type, u"Button"_qs));
        // selectors.append(Selector(Selector::SelectorType::Type, u"Background"_qs));
        //
        // QList<Element::Ptr> elements;
        // auto element = Element::create();
        // element->setId(u"window-contents"_qs);
        // elements.append(element);
        //
        // element = Element::create();
        // element->setType(u"Button"_qs);
        // elements.append(element);
        //
        // element = Element::create();
        // element->setType(u"Background"_qs);
        // elements.append(element);
        QFETCH(SelectorList, selectors);
        QFETCH(ElementList, elements);
        QFETCH(bool, expected);

        QCOMPARE(selectorListMatches(selectors, elements), expected);
    }
};

QTEST_MAIN(TestSelector)

#include "TestSelector.moc"
