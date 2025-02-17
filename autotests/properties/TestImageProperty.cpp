// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
//
// This file is automatically generated from autotest.cpp.j2.
// To regenerate, run `tools/propertygenerator/generate_properties.py`.

#include <QtTest>

#include <properties/ImageProperty.h>

#include "CreateTestInstances.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

class TestImageProperty : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEmpty()
    {
        ImageProperty property;

        // An empty instance should not have any values for its properties.
        QVERIFY(!property.imageData().has_value());
        QVERIFY(!property.width().has_value());
        QVERIFY(!property.height().has_value());
        QVERIFY(!property.xOffset().has_value());
        QVERIFY(!property.yOffset().has_value());
        QVERIFY(!property.flags().has_value());
    }

    void testHasAnyValue()
    {
        ImageProperty property;

        // An empty instance should not have any values for its properties.
        QVERIFY(!property.hasAnyValue());

        {
            QImage value;
            property.setImageData(value);
            QVERIFY(property.hasAnyValue());
            property.setImageData(std::nullopt);
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
            qreal value;
            property.setXOffset(value);
            QVERIFY(property.hasAnyValue());
            property.setXOffset(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            qreal value;
            property.setYOffset(value);
            QVERIFY(property.hasAnyValue());
            property.setYOffset(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            Union::Properties::ImageFlags value;
            property.setFlags(value);
            QVERIFY(property.hasAnyValue());
            property.setFlags(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
    }

    void testResolveProperties()
    {
        ImageProperty source;
        ImageProperty destination;

        QVERIFY(!source.hasAnyValue());
        QVERIFY(!destination.hasAnyValue());

        // Calling resolve on empty source and destination should have no effect.
        ImageProperty::resolveProperties(source, destination);

        QVERIFY(!destination.hasAnyValue());

        source.setImageData(QImage{});
        source.setWidth(qreal{});
        source.setHeight(qreal{});
        source.setXOffset(qreal{});
        source.setYOffset(qreal{});
        source.setFlags(Union::Properties::ImageFlags{});

        QVERIFY(source.hasAnyValue());
        QVERIFY(!destination.hasAnyValue());

        ImageProperty::resolveProperties(source, destination);

        QVERIFY(destination.hasAnyValue());

        QCOMPARE(destination.imageData(), source.imageData());
        QCOMPARE(destination.width(), source.width());
        QCOMPARE(destination.height(), source.height());
        QCOMPARE(destination.xOffset(), source.xOffset());
        QCOMPARE(destination.yOffset(), source.yOffset());
        QCOMPARE(destination.flags(), source.flags());
    }
};

QTEST_MAIN(TestImageProperty)

#include "TestImageProperty.moc"