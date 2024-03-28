/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "UnionPlugin.h"

#include <Style.h>

void UnionPlugin::initializeEngine(QQmlEngine *, const char *)
{
    Union::Style::instance()->load();
}
