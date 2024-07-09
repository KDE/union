/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include <QtTest>

#include <Theme.h>

using namespace Union;

class TestPlasmaSvg : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testLoad()
    {
    }
};

QTEST_MAIN(TestPlasmaSvg)

#include "TestPlasmaSvg.moc"
