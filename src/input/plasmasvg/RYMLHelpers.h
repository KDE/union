/* SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-FileCopyrightText: 2024 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QByteArray>
#include <QString>
#include <QMetaType>
#include <QMetaEnum>

#define RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS
#include <ryml.hpp>
#include <ryml_std.hpp>
#include <c4/enum.hpp>

// The actual default implementation tries to create types using operator>>.
template<typename T>
inline T nodeValue(ryml::ConstNodeRef node)
{
    T result;
    node >> result;
    return result;
}

// NOTE: The raw data for a string view will not be null terminated at the
// position where the string view is supposed to end. It cannot be used directly
// with APIs that expect a C-style string without specifying a string length.
template<>
inline QByteArrayView nodeValue<QByteArrayView>(ryml::ConstNodeRef node)
{
    return node.val();
}

template<>
inline QByteArray nodeValue<QByteArray>(ryml::ConstNodeRef node)
{
    return nodeValue<QByteArrayView>(node).toByteArray();
}

template<>
inline QString nodeValue<QString>(ryml::ConstNodeRef node)
{
    return QString::fromLatin1(nodeValue<QByteArrayView>(node));
}

// Makes it possible to get a C-style string from a QByteArray without calling
// QByteArray::constData() directly.
// We don't want to return QByteArray::constData() in a function creating
// a QByteArray because the data would be freed after the function ended.
struct CStringWrapper : public QByteArray {
    CStringWrapper(QByteArray &&other)
        : QByteArray(other)
    {
    }
    CStringWrapper(const QByteArray &other)
        : QByteArray(other)
    {
    }
    operator const char *() const noexcept
    {
        return this->constData();
    }
};

template<>
inline CStringWrapper nodeValue<CStringWrapper>(ryml::ConstNodeRef node)
{
    return nodeValue<QByteArray>(node);
}

// QMetaEnum::fromType doesn't seem to be usable,
// so we can't make a specialization of nodeValue() for Qt enums and flags.
// clang-format off
#define nodeQEnumValue(ParentScope, Enum, node)\
    static_cast<ParentScope::Enum>(\
        ParentScope::staticMetaObject.enumerator(\
            ParentScope::staticMetaObject.indexOfEnumerator(#Enum))\
                .keyToValue(nodeValue<CStringWrapper>(node)))
// clang-format on

template<>
inline Qt::Alignment nodeValue<Qt::Alignment>(ryml::ConstNodeRef node)
{
    Qt::Alignment align;

    if (!node.is_map()) {
        return align;
    }

    if (auto horizontalNode = node.find_child("horizontal"); horizontalNode.valid()) {
        auto alignHorizontal = horizontalNode.val();
        if (alignHorizontal == "left") {
            align |= Qt::AlignLeft;
        } else if (alignHorizontal == "center") {
            align |= Qt::AlignHCenter;
        } else if (alignHorizontal == "right") {
            align |= Qt::AlignRight;
        }
    }

    if (auto verticalNode = node.find_child("vertical"); verticalNode.valid()) {
        auto alignVertical = verticalNode.val();
        if (alignVertical == "top") {
            align |= Qt::AlignTop;
        } else if (alignVertical == "center") {
            align |= Qt::AlignVCenter;
        } else if (alignVertical == "bottom") {
            align |= Qt::AlignBottom;
        }
    }

    return align;
}

template<typename I, typename O, typename F>
inline void forEachEntry(const std::initializer_list<I> &input, const std::initializer_list<O *> output, ryml::ConstNodeRef &node, F callback)
{
    Q_ASSERT(input.size() == output.size());

    auto inputItr = input.begin();
    auto outputItr = output.begin();
    for (; inputItr != input.end() && outputItr != output.end(); ++inputItr, ++outputItr) {
        auto name = ryml::to_csubstr(*inputItr);
        if (ryml::ConstNodeRef childNode = node.find_child(name); childNode.valid()) {
            *(*outputItr) = callback(childNode);
        }
    }
}
