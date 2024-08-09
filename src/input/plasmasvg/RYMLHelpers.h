/* SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-FileCopyrightText: 2024 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QByteArray>
#include <QColor>
#include <QString>
#include <QUrl>
#include <QMetaType>
#include <QMetaEnum>

#define RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS
#include <ryml.hpp>
#include <ryml_std.hpp>
#include <c4/enum.hpp>

namespace c4
{

template<typename C>
inline QDebug& operator<<(QDebug& debug, basic_substring<C> s)
{
    debug << QByteArrayView{s};
    return debug;
}

// Overloading c4::from_chars allows ryml to work with more types

inline bool from_chars(ryml::csubstr buf, QUrl *v) noexcept
{
    auto url = QUrl::fromUserInput(QUtf8StringView{buf}.toString());
    if (!url.isValid()) {
        return false;
    }
    *v = std::move(url);
    return true;
}

inline bool from_chars(ryml::csubstr buf, QColor *v) noexcept
{
    auto color = QColor::fromString(buf);
    if (!color.isValid()) {
        return false;
    }
    *v = std::move(color);
    return true;
}

}

inline QDebug operator<<(QDebug debug, const ryml::ConstNodeRef &node)
{
    QDebugStateSaver stateSaver(debug);
    debug.nospace();
    debug << "ConstNodeRef(";
    debug << (const void *)&node;
    debug << ", id=" << node.id();
    debug << ", type_str=" << node.type_str();
    if (node.has_children()) {
        debug << ", num_children=" << node.num_children();
    }
    if (node.has_key()) {
        debug << ", key=" << node.key();
    }
    if (node.has_val()) {
        debug << ", val=" << node.val();
    }
    if (node.has_key_anchor()) {
        debug << ", key_anchor=" << node.key_anchor();
    }
    if (node.has_val_anchor()) {
        debug << ", val_anchor=" << node.val_anchor();
    }
    if (node.has_key_tag()) {
        debug << ", key_tag=" << node.key_tag();
    }
    if (node.has_val_tag()) {
        debug << ", val_tag=" << node.val_tag();
    }
    debug << ')';
    return debug;
}

template <typename F, typename ReturnType = std::invoke_result_t<F, ryml::ConstNodeRef>>
inline ReturnType with_child(ryml::ConstNodeRef node, c4::csubstr name, F callback)
{
    auto child = node.find_child(name);
    if (child.valid()) {
        return callback(child);
    }

    if constexpr (!std::is_void_v<ReturnType>) {
        return ReturnType{};
    }
}

// The actual default implementation tries to create types using operator>>.
template<typename T>
inline T value(ryml::ConstNodeRef node)
{
    T result;
    node >> result;
    return result;
}

// NOTE: The raw data for a string view will not be null terminated at the
// position where the string view is supposed to end. It cannot be used directly
// with APIs that expect a C-style string without specifying a string length.
template<>
inline QByteArrayView value<QByteArrayView>(ryml::ConstNodeRef node)
{
    return node.val();
}

template<>
inline QByteArray value<QByteArray>(ryml::ConstNodeRef node)
{
    return value<QByteArrayView>(node).toByteArray();
}

template<>
inline QString value<QString>(ryml::ConstNodeRef node)
{
    return QString::fromLatin1(value<QByteArrayView>(node));
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
inline CStringWrapper value<CStringWrapper>(ryml::ConstNodeRef node)
{
    return value<QByteArray>(node);
}

// QMetaEnum::fromType doesn't seem to be usable,
// so we can't make a specialization of nodeValue() for Qt enums and flags.
// clang-format off
#define NODE_Q_ENUM_VALUE(ParentScope, Enum, node)\
    static_cast<ParentScope::Enum>(\
        ParentScope::staticMetaObject.enumerator(\
            ParentScope::staticMetaObject.indexOfEnumerator(#Enum))\
                .keyToValue(value<CStringWrapper>(node)))
// clang-format on

template<>
inline Qt::Alignment value<Qt::Alignment>(ryml::ConstNodeRef node)
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
