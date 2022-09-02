/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "PlasmaSvgLoader.h"

PlasmaSvgLoader::PlasmaSvgLoader(QObject* parent)
    : Union::StyleLoader(parent)
{
}

bool PlasmaSvgLoader::load(const QUrl& url)
{
    return false;
}

std::shared_ptr<Union::StyleElement> PlasmaSvgLoader::get(const Union::ElementSelector &selector)
{
    return nullptr;
}
