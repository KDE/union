// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <filesystem>

#include <QList>
#include <QString>

#include "StyleLoader.h"
#include "StyleRule.h"

namespace Union
{
class StylePrivate
{
public:
    std::unique_ptr<StyleLoader> loader;

    QString pluginName;
    QString styleName;

    QList<StyleRule::Ptr> rules;
};

}
