// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
//
// This file is automatically generated from autotest.cpp.j2.
// To regenerate, run `tools/propertygenerator/generate_properties.py`.

#include <QtTest>

#include <properties/BackgroundProperty.h>

#include "CreateTestInstances.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

class TestBackgroundProperty : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEmpty()
    {
        BackgroundProperty property;

        // An empty instance should not have any values for its properties.
        QVERIFY(!property.color().has_value());
        QVERIFY(!property.image().has_value());
        QVERIFY(!property.border().has_value());
        QVERIFY(!property.corners().has_value());
        QVERIFY(!property.shadow().has_value());
    }

    void testHasAnyValue()
    {
        BackgroundProperty property;

        // An empty instance should not have any values for its properties.
        QVERIFY(!property.hasAnyValue());

        {
            QColor value;
            property.setColor(value);
            QVERIFY(property.hasAnyValue());
            property.setColor(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            // Assigning an empty value to a property should have no effect.
            property.setImage(ImageProperty{});
            QVERIFY(!property.hasAnyValue());

            property.setImage(testImagePropertyInstance());
            QVERIFY(property.hasAnyValue());

            property.setImage(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            // Assigning an empty value to a property should have no effect.
            property.setBorder(BorderProperty{});
            QVERIFY(!property.hasAnyValue());

            property.setBorder(testBorderPropertyInstance());
            QVERIFY(property.hasAnyValue());

            property.setBorder(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            // Assigning an empty value to a property should have no effect.
            property.setCorners(CornersProperty{});
            QVERIFY(!property.hasAnyValue());

            property.setCorners(testCornersPropertyInstance());
            QVERIFY(property.hasAnyValue());

            property.setCorners(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
        {
            // Assigning an empty value to a property should have no effect.
            property.setShadow(ShadowProperty{});
            QVERIFY(!property.hasAnyValue());

            property.setShadow(testShadowPropertyInstance());
            QVERIFY(property.hasAnyValue());

            property.setShadow(std::nullopt);
            QVERIFY(!property.hasAnyValue());
        }
    }

    void testResolveProperties()
    {
        BackgroundProperty source;
        BackgroundProperty destination;

        QVERIFY(!source.hasAnyValue());
        QVERIFY(!destination.hasAnyValue());

        // Calling resolve on empty source and destination should have no effect.
        BackgroundProperty::resolveProperties(source, destination);

        QVERIFY(!destination.hasAnyValue());

        source.setColor(QColor{});
        source.setImage(testImagePropertyInstance());
        source.setBorder(testBorderPropertyInstance());
        source.setCorners(testCornersPropertyInstance());
        source.setShadow(testShadowPropertyInstance());

        QVERIFY(source.hasAnyValue());
        QVERIFY(!destination.hasAnyValue());

        BackgroundProperty::resolveProperties(source, destination);

        QVERIFY(destination.hasAnyValue());

        QCOMPARE(destination.color(), source.color());
        QCOMPARE(destination.image(), source.image());
        QCOMPARE(destination.border(), source.border());
        QCOMPARE(destination.corners(), source.corners());
        QCOMPARE(destination.shadow(), source.shadow());
    }
};

QTEST_MAIN(TestBackgroundProperty)

#include "TestBackgroundProperty.moc"