// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "CssPlugin.h"

#include <Style.h>

#include "CssLoader.h"

using namespace Qt::StringLiterals;

CssPlugin::CssPlugin(QObject *parent)
    : Union::InputPlugin(parent)
{
}

std::shared_ptr<Union::Style> CssPlugin::createStyle(const QString &styleName) const
{
    return Union::Style::create(u"css"_s, styleName, std::make_unique<CssLoader>());
}
