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
    void testLoad()
    {
        auto defaultsPath = QFINDTESTDATA("../../src/input/css/defaults");

        // This is a bit hacky but allows us to provide defaults that the actual
        // style doesn't need to care about. Ultimately we want to do something
        // cleaner here.
        cssparser::StyleSheet styleSheet;
        styleSheet.set_root_path(fs::path(defaultsPath.toStdString()));
        styleSheet.parse_file("default.css"s);

        auto path = QFINDTESTDATA("../../src/input/css/styles/breeze");
        auto stylePath = fs::path(path.toStdString());

        styleSheet.set_root_path(stylePath);
        styleSheet.parse_file("style.css");

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
