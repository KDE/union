// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QColor>
#include <QMetaObject>

#include "union_export.h"

namespace Union
{
/*!
 * \namespace Union::Properties
 * \inmodule core
 * \ingroup core-namespaces
 *
 * Flags and enums used by property classes.
 */
namespace Properties
{
Q_NAMESPACE_EXPORT(UNION_EXPORT)

/*!
 * \enum Union::Properties::ImageFlag
 *
 * Flags used to indicate stretching or tiling behavior for images.
 *
 * Note that Repeat and Stretch are mutually exclusive.
 *
 * \value RepeatX
 *      Repeat the image on the X axis.
 * \value RepeatY
 *      Repeat the image on the Y axis.
 * \value RepeatBoth
 *      Repeat the image on both axes.
 * \value StretchX
 *      Stretch the image on the X axis.
 * \value StretchY
 *      Stretch the image on the Y axis.
 * \value StretchBoth
 *      Stretch the image on both axes.
 */
enum class ImageFlag {
    RepeatX = 1 << 0,
    RepeatY = 1 << 1,
    RepeatBoth = RepeatX | RepeatY,
    StretchX = 1 << 2,
    StretchY = 1 << 3,
    StretchBoth = StretchX | StretchY,
};
Q_ENUM_NS(ImageFlag)
Q_DECLARE_FLAGS(ImageFlags, ImageFlag)

/*!
 * \enum Union::Properties::LineStyle
 *
 * The style that should be used to render a line.
 *
 * \value Solid
 *      Render as a solid, continuous line.
 */
enum class LineStyle {
    Solid,
};
Q_ENUM_NS(LineStyle)

/*!
 * \enum Union::Properties::AlignmentContainer
 *
 */
enum class AlignmentContainer {
    Item,
    Content,
    Background,
};
Q_ENUM_NS(AlignmentContainer)

/*!
 * \enum Union::Properties::Alignment
 *
 */
enum class Alignment {
    Unspecified,
    Start,
    Center,
    End,
    Fill,
    Stack,
};
Q_ENUM_NS(Alignment)

/*
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
