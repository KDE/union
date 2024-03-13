/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "PlasmaSvgPlugin.h"

#include "PlasmaSvgLoader.h"

PlasmaSvgPlugin::PlasmaSvgPlugin(QObject *parent)
    : Union::InputPlugin(parent)
{
}

std::unique_ptr<Union::StyleLoader> PlasmaSvgPlugin::createStyleLoader(std::shared_ptr<Union::Style> style) const
{
    return std::make_unique<PlasmaSvgLoader>(style);
}
