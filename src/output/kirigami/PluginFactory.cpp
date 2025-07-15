// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "PluginFactory.h"

#include "PlatformTheme.h"
#include "Units.h"

PluginFactory::PluginFactory(QObject *parent)
    : Kirigami::Platform::PlatformPluginFactory(parent)
{
}

Kirigami::Platform::PlatformTheme *PluginFactory::createPlatformTheme(QObject *parent)
{
    return new PlatformTheme(parent);
}

Kirigami::Platform::Units *PluginFactory::createUnits(QObject *parent)
{
    return new Units(parent);
}
