// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <type_traits>

#include <QColor>
#include <QMetaEnum>
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
 * \value Mask
 *      Use the image as a mask.
 * \value InvertedMask
 *      Use the image as a mask, but invert the masked area.
 */
enum class ImageFlag {
    RepeatX = 1 << 0,
    RepeatY = 1 << 1,
    RepeatBoth = RepeatX | RepeatY,
    StretchX = 1 << 2,
    StretchY = 1 << 3,
    StretchBoth = StretchX | StretchY,
    Mask = 1 << 4,
    InvertedMask = 1 << 5
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
    None,
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
    StackCenter,
    StackFill,
};
Q_ENUM_NS(Alignment)

/*!
 * \enum Union::Properties::StringRepresentation
 */
enum class ToStringFlag {
    MultiLine = 1 << 0,
    Types = 1 << 1,
};
Q_DECLARE_FLAGS(ToStringFlags, ToStringFlag)

/*!
 * \enum Union::Properties::TextWrapMode
 */
enum class TextWrapMode {
    NoWrap,
    WordWrap,
    ManualWrap,
    WrapAnywhere,
    WrapAtWordBoundaryOrAnywhere
};
Q_ENUM_NS(TextWrapMode)

/*!
 * \enum Union::Properties::TextElide
 */
enum class TextElide {
    None,
    Left,
    Middle,
    Right
};
Q_ENUM_NS(TextElide)

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

// Simple helper concept that constraints things to be pointer-to-member-function
// that removes the need to specify this in a requires clause.
template<typename T>
concept MemberFunctionPointer = std::is_member_function_pointer_v<T>;

/*!
 * Perform safe property lookup on a property group.
 *
 * This will return the value of the property that is accessed by calling
 * \a function which is required to be a pointer-to-member-function of \a group.
 * If the value is a pointer or std::optional<>, the value is checked to be
 * valid before returning. If it is not, \a defaultValue is returned.
 */
template<typename T, typename PropertyGroup, MemberFunctionPointer LookupFunction>
    requires std::is_invocable_v<LookupFunction, PropertyGroup>
static inline T safePropertyLookup(const PropertyGroup *group, const T &defaultValue, LookupFunction function)
{
    auto value = (group->*function)();
    if constexpr (std::is_pointer_v<decltype(value)>) {
        if (value) {
            return value;
        }
    } else if constexpr (std::is_same_v<decltype(value), std::optional<T>>) {
        if (value) {
            return value.value();
        }
    } else {
        return value;
    }
    return defaultValue;
}

/*!
 * Perform safe property lookup on a property group.
 *
 * This will return the value of a property specified as final function of
 * \a functions using \a function and the other entries in \a functions to look
 * up property groups along the way. If any of the property groups return
 * nullptr, \a defaultValue will be returned.
 */
template<typename T, typename PropertyGroup, MemberFunctionPointer LookupFunction, MemberFunctionPointer... LookupFunctions>
    requires std::is_invocable_v<LookupFunction, PropertyGroup>
static inline T safePropertyLookup(const PropertyGroup *group, const T &defaultValue, LookupFunction function, LookupFunctions... functions)
{
    auto value = (group->*function)();
    if (value) {
        return safePropertyLookup(value, defaultValue, functions...);
    } else {
        return defaultValue;
    }
}
}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Union::Properties::ImageFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(Union::Properties::ToStringFlags)

// Helper templates to more easily use enums with QTextStream
template<typename T>
    requires std::is_enum_v<T>
QTextStream &operator<<(QTextStream &stream, T value)
{
    auto metaEnum = QMetaEnum::fromType<T>();
    stream << metaEnum.valueToKeys(quint64(value));
    return stream;
}

template<typename T>
    requires std::is_enum_v<T>
QTextStream &operator<<(QTextStream &stream, QFlags<T> value)
{
    auto metaEnum = QMetaEnum::fromType<T>();
    stream << metaEnum.valueToKeys(quint64(value));
    return stream;
}
