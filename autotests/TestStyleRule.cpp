/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include <QtTest>

#include <StyleRule.h>

using namespace Union;

class TestStyleRule : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testContentSize()
    {
        auto style = StyleRule::create();

        // QCOMPARE(style->contentSize(), QSizeF());

        // AreaDefinition background;
        // background.size = QSizeF(10, 10);
        // style->setBackground(background);

        // QCOMPARE(style->contentSize(), QSizeF(10, 10));

        // AreaDefinition foreground;
        // foreground.size = QSizeF(15, 15);
        // style->setForeground(foreground);

        // QCOMPARE(style->contentSize(), QSizeF(15, 15));

        // background.size = QSizeF(20, 20);
        // style->setBackground(background);

        // QCOMPARE(style->contentSize(), QSizeF(20, 20));
    }

    void testBorderSizes()
    {
        // auto style = StyleRule::create();
        //
        // QCOMPARE(style->borderSizes(), QMarginsF());
        //
        // BorderDefinition border;
        // LineDefinition line;
        // line.size = 10;
        // border.left = line;
        // border.top = line;
        // style->setBorder(border);
        //
        // QCOMPARE(style->borderSizes(), QMarginsF(10, 10, 0, 0));
        //
        // border.right = line;
        // border.bottom = line;
        // style->setBorder(border);
        //
        // QCOMPARE(style->borderSizes(), QMarginsF(10, 10, 10, 10));
    }

    void testBoundingRect()
    {
        // auto style = StyleRule::create();
        //
        // QCOMPARE(style->boundingRect(), QRectF());
        //
        // AreaDefinition background;
        // background.size = QSizeF(10, 10);
        // style->setBackground(background);
        //
        // QCOMPARE(style->boundingRect(), QRectF(0, 0, 10, 10));
        //
        // BorderDefinition border;
        // LineDefinition line;
        // line.size = 10;
        // border.left = line;
        // border.top = line;
        // style->setBorder(border);
        //
        // QCOMPARE(style->boundingRect(), QRectF(0, 0, 20, 20));
    }
};

QTEST_MAIN(TestStyleRule)

#include "TestStyleRule.moc"
