// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `generate_properties.py`.

#include <QtTest>

#include "../PaletteProperty.h"

#include "CreateTestInstances.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

class TestPaletteProperty : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEmpty()
    {
        PaletteProperty property;

        // An empty instance should have no values set.
        QVERIFY(!property.foreground().has_value());
        QVERIFY(!property.background().has_value());
        QVERIFY(!property.accent().has_value());
    }

    void testHasAnyValue()
    {
        PaletteProperty property;

        // An empty instance should not have any values.
        QVERIFY(!property.hasAnyValue());

        {
            QColor value;
            property.setForeground(value);
            QVERIFY(property.hasAnyValue());
            property.setForeground(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            QColor value;
            property.setBackground(value);
            QVERIFY(property.hasAnyValue());
            property.setBackground(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            QColor value;
            property.setAccent(value);
            QVERIFY(property.hasAnyValue());
            property.setAccent(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
    }

    void testResolveProperties()
    {
        PaletteProperty source;
        PaletteProperty destination;

        QVERIFY(!source.hasAnyValue());
        QVERIFY(!destination.hasAnyValue());

        // Calling resolve on empty source and destination should have no effect.
        PaletteProperty::resolveProperties(source, destination);

        QVERIFY(!destination.hasAnyValue());

        source.setForeground(QColor{});
        source.setBackground(QColor{});
        source.setAccent(QColor{});

        QVERIFY(source.hasAnyValue());
        QVERIFY(!destination.hasAnyValue());

        PaletteProperty::resolveProperties(source, destination);

        QVERIFY(destination.hasAnyValue());

        QCOMPARE(destination.foreground(), source.foreground());
        QCOMPARE(destination.background(), source.background());
        QCOMPARE(destination.accent(), source.accent());
    }
};

QTEST_MAIN(TestPaletteProperty)

#include "TestPaletteProperty.moc"