/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <functional>

#include <QHash>

#include <ryml.hpp>

#include "LoadingContext.h"
#include "RYMLHelpers.h"

namespace PropertyFunctions
{

struct Error {
    Error(const QAnyStringView &_m)
        : message(_m.toString())
    {
    }

    QString message;
};

template<typename T>
struct Result {
    Result(const T &value)
        : m_storage(value)
    {
    }

    Result(const Error &error)
        : m_storage(error)
    {
    }

    bool has_value() const
    {
        return std::holds_alternative<T>(m_storage);
    }

    T value() const
    {
        if (!has_value()) {
            throw std::runtime_error("Result does not have a value");
        }
        return std::get<T>(m_storage);
    }

    Error error() const
    {
        if (has_value()) {
            throw std::runtime_error("Result is not in an error state");
        }
        return std::get<Error>(m_storage);
    }

private:
    std::variant<T, Error> m_storage;
};

struct PropertyFunctionResult : public Result<QVariant> {
    using Result<QVariant>::Result;

    template<typename V>
    PropertyFunctionResult(const V &value)
        : Result{QVariant{value}}
    {
    }

    template<typename V>
    V value() const
    {
        return Result<QVariant>::value().value<V>();
    }
};

using PropertyFunction = PropertyFunctionResult (*)(ryml::ConstNodeRef, LoadingContext &);

PropertyFunctionResult constantValue(ryml::ConstNodeRef node, LoadingContext &context);
PropertyFunctionResult elementSize(ryml::ConstNodeRef node, LoadingContext &context);
PropertyFunctionResult elementWidth(ryml::ConstNodeRef node, LoadingContext &context);
PropertyFunctionResult elementHeight(ryml::ConstNodeRef node, LoadingContext &context);
PropertyFunctionResult elementImage(ryml::ConstNodeRef node, LoadingContext &context);
PropertyFunctionResult elementImageBlend(ryml::ConstNodeRef node, LoadingContext &context);
PropertyFunctionResult sum(ryml::ConstNodeRef node, LoadingContext &context);
PropertyFunctionResult iconSizeFromName(ryml::ConstNodeRef node, LoadingContext &context);

inline static QHash<QByteArray, PropertyFunction> propertyFunctions{
    {"element-size", elementSize},
    {"element-width", elementWidth},
    {"element-height", elementHeight},
    {"element-image", elementImage},
    {"element-image-blend", elementImageBlend},
    {"sum", sum},
    {"icon-size", iconSizeFromName},
};

template<typename T>
inline T constantValue(ryml::ConstNodeRef)
{
    return T{};
}

template<>
inline qreal constantValue<qreal>(ryml::ConstNodeRef node)
{
    return value<qreal>(node);
}

template<>
inline QString constantValue<QString>(ryml::ConstNodeRef node)
{
    return value<QString>(node);
}

template<>
inline QColor constantValue<QColor>(ryml::ConstNodeRef node)
{
    return value<QColor>(node);
}

template<>
inline QUrl constantValue<QUrl>(ryml::ConstNodeRef node)
{
    return value<QUrl>(node);
}

template<typename T>
Result<T> elementProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (node.has_val()) {
        return constantValue<T>(node);
    }

    auto propertyNode = node.find_child("property");
    if (!propertyNode.readable()) {
        return Error{"Key 'property' not found"};
    }

    auto name = value<QByteArrayView>(propertyNode);
    if (name.isEmpty()) {
        return Error{"Key 'property' is empty"};
    }

    if (name == "constant") {
        auto valueNode = node.find_child("value");
        return valueNode.readable() ? Result{constantValue<T>(valueNode)} : Error{"Key 'value' not found"};
    }

    auto itr = std::find_if(propertyFunctions.keyValueBegin(), propertyFunctions.keyValueEnd(), [name](auto entry) {
        return name == entry.first;
    });

    if (itr == propertyFunctions.keyValueEnd()) {
        return Error{"No property function named " + name + " could be found"};
    }

    auto cleanup = context.pushFromNode(node);

    auto result = itr->second(node, context);
    if (result.has_value()) {
        return result.template value<T>();
    } else {
        return result.error();
    }
}

}
