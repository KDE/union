/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QObject>

#include "StyleLoader.h"

#include "union_export.h"

namespace Union
{

class UNION_EXPORT InputPlugin : public QObject
{
    Q_OBJECT

public:
    InputPlugin(QObject *parent = nullptr);

    virtual std::unique_ptr<StyleLoader> createStyleLoader(std::shared_ptr<Style> style) const = 0;
};

}

Q_DECLARE_INTERFACE(Union::InputPlugin, "org.kde.union.InputPlugin")
Q_DECLARE_METATYPE(Union::InputPlugin)
