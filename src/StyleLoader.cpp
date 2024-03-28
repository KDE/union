/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "StyleLoader.h"

#include <unordered_map>
#include <vector>

#include "ElementIdentifier.h"
#include "Style.h"

using namespace Union;

StyleLoader::StyleLoader(std::shared_ptr<Style> style, const QString &name, QObject *parent)
    : QObject(parent)
    , m_style(style)
    , m_name(name)
{
}

StyleLoader::~StyleLoader() noexcept
{
}

std::shared_ptr<Style> StyleLoader::style() const
{
    return m_style;
}

QString StyleLoader::name() const
{
    return m_name;
}
