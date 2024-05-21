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

/**
 * Abstract base class for plugins providing input data.
 *
 * This class defines the interface input plugins should implement. Input
 * plugins are plugins providing style rules to the themeing system.
 */
class UNION_EXPORT InputPlugin : public QObject
{
    Q_OBJECT

public:
    InputPlugin(QObject *parent = nullptr);

    virtual std::unique_ptr<StyleLoader> createStyleLoader(std::shared_ptr<Theme> theme) const = 0;
};

}

Q_DECLARE_INTERFACE(Union::InputPlugin, "org.kde.union.InputPlugin")
Q_DECLARE_METATYPE(Union::InputPlugin)
