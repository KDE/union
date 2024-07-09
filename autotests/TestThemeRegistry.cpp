/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include <QtTest>

#include <ThemeRegistry.h>

using namespace Union;
using namespace Qt::StringLiterals;

class TestThemeRegistry : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    // TODO: Test plugin loading functionality
};

QTEST_MAIN(TestThemeRegistry)

#include "TestThemeRegistry.moc"
