/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "StyleLoader.h"

#include <unordered_map>
#include <vector>

#include "Style.h"
#include "Theme.h"

using namespace Union;

StyleLoader::StyleLoader(std::shared_ptr<Theme> theme, const QString &name, QObject *parent)
    : QObject(parent)
    , m_theme(theme)
    , m_name(name)
{
}

StyleLoader::~StyleLoader() noexcept
{
}

std::shared_ptr<Theme> StyleLoader::theme() const
{
    return m_theme;
}

QString StyleLoader::name() const
{
    return m_name;
}
