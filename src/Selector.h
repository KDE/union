/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>

#include <QList>
#include <QString>
#include <QVariant>

#include "Element.h"

#include "union_export.h"

namespace Union
{

class Selector;
using SelectorList = QList<Selector>;

enum class SelectorType {
    Empty,
    Type,
    Id,
    State,
    ColorSet,
    Hint,
    Attribute,
    AnyOf,
    AllOf,
};

namespace detail
{
// Clang-format insists on moving the template declarations to their own line which makes this utterly unreadable.
/* clang-format off */
    template <SelectorType type, typename T> constexpr bool ArgumentTypesMatch = false;
    template <typename T> constexpr bool ArgumentTypesMatch<SelectorType::Type, T> = std::is_same_v<T, QString>;
    template <typename T> constexpr bool ArgumentTypesMatch<SelectorType::Id, T> = std::is_same_v<T, QString>;
    template <typename T> constexpr bool ArgumentTypesMatch<SelectorType::State, T> = std::is_same_v<T, Element::State>;
    template <typename T> constexpr bool ArgumentTypesMatch<SelectorType::ColorSet, T> = std::is_same_v<T, Element::ColorSet>;
    template <typename T> constexpr bool ArgumentTypesMatch<SelectorType::Hint, T> = std::is_same_v<T, QString>;
    template <typename T> constexpr bool ArgumentTypesMatch<SelectorType::Attribute, T> = std::is_same_v<T, std::pair<QString, QVariant>>;
    template <typename T> constexpr bool ArgumentTypesMatch<SelectorType::AnyOf, T> = std::is_same_v<T, SelectorList>;
    template <typename T> constexpr bool ArgumentTypesMatch<SelectorType::AllOf, T> = std::is_same_v<T, SelectorList>;
/* clang-format on */
}

class UNION_EXPORT Selector
{
public:
    bool isValid() const;

    bool matches(std::shared_ptr<Element> element) const;
    QString toString() const;

    inline static Selector create()
    {
        return Selector(nullptr);
    }

    template<SelectorType type, typename DataType>
        requires detail::ArgumentTypesMatch<type, std::decay_t<DataType>>
    static Selector create(DataType &&data)
    {
        return Selector(std::make_shared<SelectorPrivateImpl<type, std::decay_t<DataType>>>(std::forward<DataType>(data)));
    }

private:
    struct SelectorPrivate {
        virtual ~SelectorPrivate();
        virtual bool matches(std::shared_ptr<Element> element) const = 0;
        virtual QString toString() const = 0;
    };

    template<SelectorType _type, typename T>
    struct SelectorPrivateImpl : public SelectorPrivate {
        SelectorPrivateImpl(const T &_data)
            : data(_data)
        {
        }

        bool matches(std::shared_ptr<Element> element) const override;
        QString toString() const override;

        SelectorType type = _type;
        T data;
    };

    Selector(std::shared_ptr<const SelectorPrivate> _d);

    std::shared_ptr<const SelectorPrivate> d;
};

class UNION_EXPORT SelectorList : public QList<Selector>
{
public:
    using QList::QList;

    bool matches(const QList<std::shared_ptr<Element>> &elements) const;
}

UNION_EXPORT QDebug
operator<<(QDebug debug, const Union::Selector &selector);
UNION_EXPORT QDebug operator<<(QDebug debug, const Union::SelectorList &selectors);
