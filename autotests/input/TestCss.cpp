// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <QtTest>

#include <QStandardPaths>

#include <Style.h>

#include <CssParser.h>

namespace fs = std::filesystem;

using namespace Union;
using namespace Qt::StringLiterals;
using namespace std::string_literals;

class TestCss : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testLoad_data()
    {
        QTest::addColumn<std::string>("styleName");

        QTest::addRow("breeze") << "breeze"s;
        QTest::addRow("breeze-mobile") << "breeze-mobile"s;
        QTest::addRow("breeze-rtl") << "breeze-rtl"s;
    }

    void testLoad()
    {
        QFETCH(std::string, styleName);

        auto cssInputPath = fs::path(QFINDTESTDATA("../../src/input/css/").toStdString());

        cssparser::StyleSheet styleSheet(cssInputPath / "styles"s / styleName / "style.css"s);
        styleSheet.import(cssInputPath / "defaults"s / "default.css"s);
        styleSheet.parse();

        if (styleSheet.errors().size() > 0) {
            qWarning() << "Errors encountered while parsing CSS:";
            for (const auto &error : styleSheet.errors()) {
                qWarning() << error.message.data();
            }

            QFAIL("Parsing stylesheet produced errors!");
        }
    }
};

QTEST_MAIN(TestCss)

#include "TestCss.moc"
