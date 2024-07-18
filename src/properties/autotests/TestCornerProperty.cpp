// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `generate_properties.py`.

#include <QtTest>

#include "../CornerProperty.h"

#include "CreateTestInstances.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

class TestCornerProperty : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEmpty()
    {
        CornerProperty property;

        // An empty instance should have no values set.
        QVERIFY(!property.radius().has_value());
        QVERIFY(!property.width().has_value());
        QVERIFY(!property.height().has_value());
        QVERIFY(!property.color().has_value());
        QVERIFY(!property.image().has_value());
    }

    void testHasAnyValue()
    {
        CornerProperty property;

        // An empty instance should not have any values.
        QVERIFY(!property.hasAnyValue());

        {
            qreal value;
            property.setRadius(value);
            QVERIFY(property.hasAnyValue());
            property.setRadius(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
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
            QColor value;
            property.setColor(value);
            QVERIFY(property.hasAnyValue());
            property.setColor(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            // Assigning an empty property to a value should have no effect.
            property.setImage(ImageProperty{});
            QVERIFY(!property.hasAnyValue());

            property.setImage(testImagePropertyInstance());
            QVERIFY(property.hasAnyValue());

            property.setImage(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
    }

    void testResolveProperties()
    {
        CornerProperty source;
        CornerProperty destination;

        QVERIFY(!source.hasAnyValue());
        QVERIFY(!destination.hasAnyValue());

        // Calling resolve on empty source and destination should have no effect.
        CornerProperty::resolveProperties(source, destination);

        QVERIFY(!destination.hasAnyValue());

        source.setRadius(qreal{});
        source.setWidth(qreal{});
        source.setHeight(qreal{});
        source.setColor(QColor{});
        source.setImage(testImagePropertyInstance());

        QVERIFY(source.hasAnyValue());
        QVERIFY(!destination.hasAnyValue());

        CornerProperty::resolveProperties(source, destination);

        QVERIFY(destination.hasAnyValue());

        QCOMPARE(destination.radius(), source.radius());
        QCOMPARE(destination.width(), source.width());
        QCOMPARE(destination.height(), source.height());
        QCOMPARE(destination.color(), source.color());
        QCOMPARE(destination.image(), source.image());
    }
};

QTEST_MAIN(TestCornerProperty)

#include "TestCornerProperty.moc"