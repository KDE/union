/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "PlasmaSvgPlugin.h"

#include <Theme.h>

#include "PlasmaSvgLoader.h"

using namespace Qt::StringLiterals;

PlasmaSvgPlugin::PlasmaSvgPlugin(QObject *parent)
    : Union::InputPlugin(parent)
{
}

std::shared_ptr<Union::Theme> PlasmaSvgPlugin::createTheme(const QString &themeName) const
{
    return Union::Theme::create(u"plasmasvg"_qs, themeName, std::make_unique<PlasmaSvgLoader>());
}
