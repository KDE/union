// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `tools/propertygenerator/generate_properties.py`.

#include <QtTest>

#include <properties/LayoutProperty.h>

#include "CreateTestInstances.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

class TestLayoutProperty : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEmpty()
    {
        LayoutProperty property;

        // An empty instance should have no values set.
        QVERIFY(!property.width().has_value());
        QVERIFY(!property.height().has_value());
        QVERIFY(!property.spacing().has_value());
        QVERIFY(!property.alignment().has_value());
        QVERIFY(!property.padding().has_value());
        QVERIFY(!property.margins().has_value());
    }

    void testHasAnyValue()
    {
        LayoutProperty property;

        // An empty instance should not have any values.
        QVERIFY(!property.hasAnyValue());

        {
            qreal value;
            property.setWidth(value);
            QVERIFY(property.hasAnyValue());
            property.setWidth(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            qreal value;
            property.setHeight(value);
            QVERIFY(property.hasAnyValue());
            property.setHeight(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            qreal value;
            property.setSpacing(value);
            QVERIFY(property.hasAnyValue());
            property.setSpacing(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            Qt::Alignment value;
            property.setAlignment(value);
            QVERIFY(property.hasAnyValue());
            property.setAlignment(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            // Assigning an empty property to a value should have no effect.
            property.setPadding(SizeProperty{});
            QVERIFY(!property.hasAnyValue());

            property.setPadding(testSizePropertyInstance());
            QVERIFY(property.hasAnyValue());

            property.setPadding(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            // Assigning an empty property to a value should have no effect.
            property.setMargins(SizeProperty{});
            QVERIFY(!property.hasAnyValue());

            property.setMargins(testSizePropertyInstance());
            QVERIFY(property.hasAnyValue());

            property.setMargins(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
    }

    void testResolveProperties()
    {
        LayoutProperty source;
        LayoutProperty destination;

        QVERIFY(!source.hasAnyValue());
        QVERIFY(!destination.hasAnyValue());

        // Calling resolve on empty source and destination should have no effect.
        LayoutProperty::resolveProperties(source, destination);

        QVERIFY(!destination.hasAnyValue());

        source.setWidth(qreal{});
        source.setHeight(qreal{});
        source.setSpacing(qreal{});
        source.setAlignment(Qt::Alignment{});
        source.setPadding(testSizePropertyInstance());
        source.setMargins(testSizePropertyInstance());

        QVERIFY(source.hasAnyValue());
        QVERIFY(!destination.hasAnyValue());

        LayoutProperty::resolveProperties(source, destination);

        QVERIFY(destination.hasAnyValue());

        QCOMPARE(destination.width(), source.width());
        QCOMPARE(destination.height(), source.height());
        QCOMPARE(destination.spacing(), source.spacing());
        QCOMPARE(destination.alignment(), source.alignment());
        QCOMPARE(destination.padding(), source.padding());
        QCOMPARE(destination.margins(), source.margins());
    }
};

QTEST_MAIN(TestLayoutProperty)

#include "TestLayoutProperty.moc"