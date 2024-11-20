/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QColor>
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

enum class AlignmentContainer {
    Item,
    Content,
    Background,
};
Q_ENUM_NS(AlignmentContainer)

enum class Alignment {
    Unspecified,
    Start,
    Center,
    End,
    Fill,
    Stack,
};
Q_ENUM_NS(Alignment)

/*!
 * A template method to get an empty value of a certain type.
 *
 * By default this will just default-construct the type but it allows us to use
 * template specialization to specify what an empty value means for a specific
 * type.
 */
template<typename T>
inline T emptyValue()
{
    return T{};
}

template<>
inline qreal emptyValue<qreal>()
{
    return 0.0;
}

template<>
inline int emptyValue<int>()
{
    return 0;
}

// A default-constructed QColor is black, which is not actually empty.
// So instead use transparent for an empty QColor.
template<>
inline QColor emptyValue<QColor>()
{
    return Qt::transparent;
}
}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Union::Properties::ImageFlags)
