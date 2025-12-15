// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <QtTest>

#include <StyleRegistry.h>

using namespace Union;
using namespace Qt::StringLiterals;

class TestStyleRegistry : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    // TODO: Test plugin loading functionality
};

QTEST_MAIN(TestStyleRegistry)

#include "TestStyleRegistry.moc"
