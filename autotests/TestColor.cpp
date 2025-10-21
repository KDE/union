// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <QtTest>

#include <Color.h>

using namespace Union;
using namespace Qt::StringLiterals;

class TestColorProvider : public ColorProvider
{
public:
    std::optional<ColorProvider::Rgba> color(const QStringList &arguments) const override
    {
        if (arguments.size() != 1) {
            return std::nullopt;
        }

        if (arguments.first() == u"red") {
            return Rgba{.r = 255, .g = 0, .b = 0, .a = 255};
        } else if (arguments.first() == u"green") {
            return Rgba{.r = 0, .g = 255, .b = 0, .a = 255};
        } else if (arguments.first() == u"blue") {
            return Rgba{.r = 0, .g = 0, .b = 255, .a = 255};
        }

        return std::nullopt;
    }
};

class TestColor : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        ColorProvider::registerProvider(u"test"_s, new TestColorProvider);
    }

    void testCreate()
    {
        Color color;
        QVERIFY(!color.isValid());
        QVERIFY(!color.toRgba().isValid());

        auto rgba = Color::rgba(255, 0, 0, 255);
        QVERIFY(rgba.isValid());
        QCOMPARE(rgba.type(), ColorData::Type::RGBA);
        QCOMPARE(rgba.red(), 255);
        QCOMPARE(rgba.green(), 0);
        QCOMPARE(rgba.blue(), 0);
        QCOMPARE(rgba.alpha(), 255);

        auto custom = Color::custom(u"test"_s, {u"red"_s});
        QVERIFY(custom.isValid());
        QCOMPARE(custom.type(), ColorData::Type::Custom);

        auto mix = Color::mix(rgba, custom, 0.5);
        QVERIFY(mix.isValid());
        QCOMPARE(mix.type(), ColorData::Type::MixOperation);
    }

    void testCustom()
    {
        auto color = Color::custom(u"test"_s, {u"red"_s});
        QVERIFY(color.isValid());

        auto rgba = color.toRgba();
        QVERIFY(rgba.isValid());

        QCOMPARE(rgba.red(), 255);
        QCOMPARE(rgba.green(), 0);
        QCOMPARE(rgba.blue(), 0);
        QCOMPARE(rgba.alpha(), 255);
    }

    void testAdd()
    {
        auto color = Color::rgba(127, 127, 127, 127);
        auto other = Color::rgba(100, 100, 100, 100);
        auto add = Color::add(color, other);

        auto rgba = add.toRgba();
        QVERIFY(rgba.isValid());
        QCOMPARE(rgba.red(), 227);
        QCOMPARE(rgba.green(), 227);
        QCOMPARE(rgba.blue(), 227);
        QCOMPARE(rgba.alpha(), 227);
    }

    void testSubtract()
    {
        auto color = Color::rgba(127, 127, 127, 127);
        auto other = Color::rgba(100, 100, 100, 100);
        auto sub = Color::subtract(color, other);

        auto rgba = sub.toRgba();
        QVERIFY(rgba.isValid());
        QCOMPARE(rgba.red(), 27);
        QCOMPARE(rgba.green(), 27);
        QCOMPARE(rgba.blue(), 27);
        QCOMPARE(rgba.alpha(), 27);
    }

    void testMultiply()
    {
        auto color = Color::rgba(127, 127, 127, 127);
        auto other = Color::rgba(127, 127, 127, 127);
        auto mul = Color::multiply(color, other);

        auto rgba = mul.toRgba();
        QVERIFY(rgba.isValid());
        QCOMPARE(rgba.red(), 63);
        QCOMPARE(rgba.green(), 63);
        QCOMPARE(rgba.blue(), 63);
        QCOMPARE(rgba.alpha(), 63);
    }

    void testSet()
    {
        auto color = Color::rgba(127, 127, 127, 127);
        auto set = Color::set(color, std::nullopt, 255, std::nullopt, 255);

        auto rgba = set.toRgba();
        QVERIFY(rgba.isValid());
        QCOMPARE(rgba.red(), 127);
        QCOMPARE(rgba.green(), 255);
        QCOMPARE(rgba.blue(), 127);
        QCOMPARE(rgba.alpha(), 255);
    }

    void testMix()
    {
        auto first = Color::rgba(255, 0, 0, 255);
        auto second = Color::rgba(0, 255, 0, 255);
        auto mix = Color::mix(first, second, 0.5);

        auto rgba = mix.toRgba();
        QVERIFY(rgba.isValid());
        QCOMPARE(rgba.red(), 127);
        QCOMPARE(rgba.green(), 127);
        QCOMPARE(rgba.blue(), 0);
        QCOMPARE(rgba.alpha(), 255);

        auto firstCustom = Color::custom(u"test"_s, {u"green"_s});
        auto secondCustom = Color::custom(u"test"_s, {u"blue"_s});
        auto mixedCustom = Color::mix(firstCustom, secondCustom, 0.25);

        auto customRgba = mixedCustom.toRgba();
        QVERIFY(customRgba.isValid());
        QCOMPARE(customRgba.red(), 0);
        QCOMPARE(customRgba.green(), 191);
        QCOMPARE(customRgba.blue(), 63);
        QCOMPARE(customRgba.alpha(), 255);
    }
};

QTEST_MAIN(TestColor)

#include "TestColor.moc"
