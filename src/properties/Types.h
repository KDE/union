/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QMetaObject>

#include "union_export.h"

namespace Union
{
namespace Properties
{
Q_NAMESPACE_EXPORT(UNION_EXPORT)

enum class ImageFlag {
    None = 0, //
    RepeatX = 1 << 0, //
    RepeatY = 1 << 1, //
    RepeatBoth = RepeatX | RepeatY, //
    StretchX = 1 << 2, //
    StretchY = 1 << 3, //
    StretchBoth = StretchX | StretchY, //
};
Q_ENUM_NS(ImageFlag)
Q_DECLARE_FLAGS(ImageFlags, ImageFlag)

enum class LineStyle {
    Solid,
    Dashed,
};
Q_ENUM_NS(LineStyle)

}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Union::Properties::ImageFlags)
