// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

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
class SelectorList;

/*!
 * \enum Union::SelectorType
 * \relates Union::Selector
 *
 * The type of selector.
 *
 * \value Empty
 *      An empty selector, which does nothing.
 * \value Type
 *      A selector matching on the \l{Union::Element::type}{type} property.
 *      Checks if the specified string is equal to that of the property.
 * \value Id
 *      A selector matching on the \l{Union::Element::id}{id} property.
 *      Checks if the specified string is equal to that of the property.
 * \value State
 *      A selector matching on the \l{Union::Element::state}{state} property.
 *      Checks if the specified state is set in the property.
 * \value ColorSet
 *
 * \value Hint
 *
 * \value Attribute
 *      A selector matching on an attribute in the
 *      \l{Union::Element::attributes}{attributes} property. Checks if the value
 *      of the attribute matches the specified value.
 * \value AnyOf
 *      A selector that will match if any of the specified selectors match.
 * \value AllOf
 *      A selector that will match only if all of the specified selectors match.
 */
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

/*!
 * \class Union::Selector
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief A class to handle matching some data to an element.
 */
class UNION_EXPORT Selector
{
public:
    /*!
     * Returns whether this selector is valid.
     *
     * An invalid selector will never match anything.
     */
    bool isValid() const;

    /*!
     * Returns the weight of this selector.
     */
    int weight() const;

    /*!
     * Returns whether this selector matches an element.
     *
     * \a element The element to match against.
     */
    bool matches(std::shared_ptr<Element> element) const;

    /*!
     * Return a string representation of this selector.
     */
    QString toString() const;

    /*!
     * Create an empty, invalid selector.
     */
    inline static Selector create()
    {
        return Selector(nullptr);
    }

    /*!
     * \overload Union::Selector::create()
     *
     * Create a new selector of a specific type.
     *
     * See \l[CPP]{Union::SelectorType}{SelectorType} for the available types.
     *
     * \a data The data to match on.
     */
    template<SelectorType type, typename DataType>
        requires detail::ArgumentTypesMatch<type, std::decay_t<DataType>>
    static Selector create(DataType &&data)
    {
        return Selector(std::make_shared<SelectorPrivateImpl<type, std::decay_t<DataType>>>(std::forward<DataType>(data)));
    }

private:
    struct SelectorPrivate {
        virtual ~SelectorPrivate();
        virtual int weight() const = 0;
        virtual bool matches(std::shared_ptr<Element> element) const = 0;
        virtual QString toString() const = 0;
    };

    template<SelectorType _type, typename T>
    struct SelectorPrivateImpl : public SelectorPrivate {
        SelectorPrivateImpl(const T &_data)
            : data(_data)
        {
        }

        int weight() const override;
        bool matches(std::shared_ptr<Element> element) const override;
        QString toString() const override;

        SelectorType type = _type;
        T data;
    };

    Selector(std::shared_ptr<const SelectorPrivate> _d);

    std::shared_ptr<const SelectorPrivate> d;
};

/*!
 * \class Union::SelectorList
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief A list of \l Union::Selector
 */
class UNION_EXPORT SelectorList : public QList<Selector>
{
public:
    using QList::QList;

    /*!
     * Returns the combined weight of all the selectors in the list.
     */
    int weight() const;
    /*!
     * Returns if the selectors in this list match the list of elements.
     */
    bool matches(const QList<std::shared_ptr<Element>> &elements) const;

    void appendAnyOf(const SelectorList &selectors);
    void appendAllOf(const SelectorList &selectors);
};
}

/*!
 * \relates Union::Selector
 *
 * QDebug support for Selector.
 */
UNION_EXPORT QDebug operator<<(QDebug debug, const Union::Selector &selector);
/*!
 * \relates Union::SelectorList
 *
 * QDebug support for SelectorList.
 */
UNION_EXPORT QDebug operator<<(QDebug debug, const Union::SelectorList &selectors);
