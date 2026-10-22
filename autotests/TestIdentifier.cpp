// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <QtTest>

#include <Identifier.h>

using namespace Union;
using namespace Qt::StringLiterals;

class TestIdentifier : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testCompare_data()
    {
        QTest::addColumn<Identifier>("first");
        QTest::addColumn<Identifier>("second");
        QTest::addColumn<bool>("expectedEqual");

        QTest::addRow("simple") << Identifier(u"SimpleCase") << Identifier(u"SimpleCase") << true;
        QTest::addRow("mixed case") << Identifier(u"MiXeDcAsE") << Identifier(u"mIxEdCaSe") << true;
        QTest::addRow("ignore dashes") << Identifier(u"ignore-all-these-dashes") << Identifier(u"IgnoreAllTheseDashes") << true;

        QTest::addRow("empty identifier") << Identifier{} << Identifier{} << false;
        QTest::addRow("different starting letters") << Identifier(u"first") << Identifier(u"second") << false;
        QTest::addRow("underscores") << Identifier(u"underscores_should_not_be_ignored") << Identifier(u"UnderscoresShouldNotBeIgnored") << false;
    }

    void testCompare()
    {
        QFETCH(Identifier, first);
        QFETCH(Identifier, second);
        QFETCH(bool, expectedEqual);

        auto firstHash = qHash(first);
        auto secondHash = qHash(second);

        if (expectedEqual) {
            QCOMPARE(first, second);
            QCOMPARE(firstHash, secondHash);
        } else {
            QCOMPARE_NE(first, second);
            QCOMPARE_NE(firstHash, secondHash);
        }
    }

    void testConversion()
    {
        Identifier id{u"SomeIdentifier"};

        QCOMPARE(id, u"SomeIdentifier"_s);
        QCOMPARE(id, u"some-identifier");
        QCOMPARE_NE(QString{id}, u"some-identifier"_s);
    }
};

QTEST_MAIN(TestIdentifier)

#include "TestIdentifier.moc"
