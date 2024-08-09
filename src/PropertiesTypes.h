/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QMetaObject>

#include "union_export.h"

/**
 * Flags and enums used by property classes.
 */

namespace Union
{
namespace Properties
{
Q_NAMESPACE_EXPORT(UNION_EXPORT)

/**
 * Flags used to indicate stretching or tiling behaviour for images.
 */
enum class ImageFlag {
    RepeatX = 1 << 0, ///< Repeat the image on the X axis.
    RepeatY = 1 << 1, ///< Repeat the image on the Y axis.
    RepeatBoth = RepeatX | RepeatY, ///< Repeat the image on both axes.
    StretchX = 1 << 2, ///< Stretch the image on the X axis.
    StretchY = 1 << 3, ///< Stretch the image on the Y axis;
    StretchBoth = StretchX | StretchY, ///< Stretch the image on both axes.
};
Q_ENUM_NS(ImageFlag)
Q_DECLARE_FLAGS(ImageFlags, ImageFlag)

/**
 * An enum indicating the style that should be used to render a line.
 */
enum class LineStyle {
    Solid, ///< Render as a solid, continuous line.
};
Q_ENUM_NS(LineStyle)

}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Union::Properties::ImageFlags)
