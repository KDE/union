// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <QtTest>

#include "Mnemonics.h"

using namespace Union;
using namespace Qt::StringLiterals;
using namespace std::string_literals;

class TestMnemonics : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testLabel_data()
    {
        QTest::addColumn<QString>("label");
        QTest::addColumn<QString>("expectedRichText");
        QTest::addColumn<QString>("expectedMnemonic");

        QTest::addRow("no markers") << u"Example string for testing"_s << u"Example string for testing"_s << u"&Example string for testing"_s;
        QTest::addRow("single &") << u"Examples & Testing"_s << u"Examples & Testing"_s << u"&Examples && Testing"_s;
        QTest::addRow("& for mnemonic") << u"E&xample string for testing"_s << u"Example string for testing"_s << u"E&xample string for testing"_s;
        QTest::addRow("escaped &") << u"E&&xample string for testing"_s << u"E&xample string for testing"_s << u"&E&&xample string for testing"_s;
        QTest::addRow("html entities") << u"&lt;h1&gt;Examples & Testing"_s << u"&lt;h1&gt;Examples & Testing"_s << u"&&lt;&h1&&gt;Examples && Testing"_s;
        QTest::addRow("rich text") << u"<b>Examples</b> <i>&</i> <u>Testing</u>"_s << u"<b>Examples</b> <i>&</i> <u>Testing</u>"_s
                                   << u"<b>&Examples</b> <i>&</i> <u>Testing</u>"_s;
    }

    void testLabel()
    {
        QFETCH(QString, label);
        QFETCH(QString, expectedRichText);
        QFETCH(QString, expectedMnemonic);

        Union::Mnemonics mnemonics;
        mnemonics.setLabel(label);

        QCOMPARE(mnemonics.richTextLabel(), expectedRichText);
        QCOMPARE(mnemonics.mnemonicLabel(), expectedMnemonic);
    }
};

QTEST_MAIN(TestMnemonics)

#include "TestMnemonics.moc"
