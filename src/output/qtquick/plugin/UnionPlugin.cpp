/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "UnionPlugin.h"

#include <Theme.h>

UnionPlugin::UnionPlugin(QObject *parent)
    : QQmlEngineExtensionPlugin(parent)
{
}

void UnionPlugin::initializeEngine(QQmlEngine *, const char *)
{
    volatile auto registration = &qml_register_types_org_kde_union_impl;
    Q_UNUSED(registration);

    Union::Theme::instance()->load();
}
