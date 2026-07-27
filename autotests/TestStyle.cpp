// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <QtTest>

#include <Style.h>
#include <StyleLoader.h>

#include "TestInputPlugin.h"

using namespace Union;
using namespace Qt::StringLiterals;

class TestStyle : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testLoad()
    {
        auto style = Style::create(FINDTESTPATH("TestStylePackageData/ValidTestStyle"), std::make_unique<TestStyleLoader>());

        QVERIFY(style->load());

        const auto rules = style->rules();

        QCOMPARE(rules.size(), 2);
    }

    void testMatches()
    {
        auto style = Style::create(FINDTESTPATH("TestStylePackageData/ValidTestStyle"), std::make_unique<TestStyleLoader>());
        QVERIFY(style->load());

        QList<Element::Ptr> elements;
        auto element = Element::create();
        element->setId(u"test"_s);
        elements.append(element);

        auto result = style->matches(elements);
        QCOMPARE(result.size(), 1);

        elements.clear();
        element = Element::create();
        element->setType(u"test"_s);
        elements.append(element);

        result = style->matches(elements);
        QCOMPARE(result.size(), 1);

        elements.clear();
        element = Element::create();
        element->setHint(u"test"_s);
        elements.append(element);

        result = style->matches(elements);
        QCOMPARE(result.size(), 0);
    }
};

QTEST_MAIN(TestStyle)

#include "TestStyle.moc"
