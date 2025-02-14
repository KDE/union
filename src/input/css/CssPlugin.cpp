// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "CssPlugin.h"

#include <Theme.h>

#include "CssLoader.h"

using namespace Qt::StringLiterals;

CssPlugin::CssPlugin(QObject *parent)
    : Union::InputPlugin(parent)
{
}

std::shared_ptr<Union::Theme> CssPlugin::createTheme(const QString &themeName) const
{
    return Union::Theme::create(u"css"_s, themeName, std::make_unique<CssLoader>());
}
