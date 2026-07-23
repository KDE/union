// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <QtTest>

#include <InputPlugin.h>
#include <Style.h>
#include <StyleLoader.h>
#include <StylePackage.h>

#include "TestInputPlugin.h"

using namespace Union;
using namespace Qt::StringLiterals;

namespace fs = std::filesystem;

class TestStylePackage : public QObject
{
    Q_OBJECT

    StylePackage makePackage(QStringView relativePath)
    {
        QString findPath = u"TestStylePackageData/"_s + relativePath;
        return StylePackage{FINDTESTPATH(findPath)};
    }

private Q_SLOTS:
    void initTestCase()
    {
        InputPlugin::addInputType(u"test"_s, std::make_shared<TestInputPlugin>());
    }

    void testIsValid()
    {
        auto package = std::make_unique<StylePackage>(fs::path());
        QVERIFY(!package->isValid());
        QCOMPARE(package->error(), StylePackage::Error::NotFound);

        auto testPackage = [this](QStringView style, StylePackage::Error expectedError) {
            auto package = makePackage(style);
            if (expectedError == StylePackage::Error::None) {
                QVERIFY(package.isValid());
            } else {
                QVERIFY(!package.isValid());
            }
            QCOMPARE(package.error(), expectedError);
        };

        testPackage(u"EmptyStyle", StylePackage::Error::MissingFiles);
        testPackage(u"NoContentsStyle", StylePackage::Error::MissingFiles);
        testPackage(u"EmptyMetaDataStyle", StylePackage::Error::InvalidMetaData);
        testPackage(u"UnknownInputStyle", StylePackage::Error::UnknownInputType);
        testPackage(u"ValidTestStyle", StylePackage::Error::None);
    }

    void testMetaData()
    {
        auto package = makePackage(u"ValidTestStyle");

        QVERIFY(package.isValid());

        QCOMPARE(package.id(), u"ValidTestStyle"_s);
        QCOMPARE(package.inputType(), u"test"_s);
        QCOMPARE(package.name(), u"Valid Test Style"_s);
        QCOMPARE(package.description(), u"A style used for testing that is completely valid."_s);
        QCOMPARE(package.version(), u"1.0"_s);
        QCOMPARE(package.license(), u"BSD-2-Clause"_s);
        QCOMPARE(package.authors(), (QStringList{u"Test Author 1"_s, u"Test Author 2"_s}));
    }

    void testLoad()
    {
        auto package = makePackage(u"ValidTestStyle");

        auto style = package.load();
        QVERIFY(style);
        QCOMPARE(style->path(), package.path());

        package = makePackage(u"UnknownInputStyle");

        style = package.load();
        QVERIFY(!style);
    }

    void benchmarkValidate()
    {
        QBENCHMARK {
            auto package = makePackage(u"ValidTestStyle");
            QVERIFY(package.isValid());
        }
    }
};

QTEST_MAIN(TestStylePackage)

#include "TestStylePackage.moc"
