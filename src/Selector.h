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
 * \value AnyElement
 *      A selector that matches anything.
 *      Note that this has a low weight and most other selectors will override it.
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
    AnyElement,
    ChildCombinator,
    DescendantCombinator,
    AnyOf,
    AllOf,
};

namespace detail
{

// A placeholder struct for selectors that don't have any data.
struct Empty {
};

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

// Partial type-erasure implementation for Selector.
// We want to store the concrete data type that is used by the selector,
// rather than to relying on something like QVariant, to avoid having to
// constantly convert from and to QVariant. This means we need a base class
// for the private implementation details. To avoid having to write a lot of
// boilerplate, we can use a template class for the concrete implementation
// that we specialize for the specific selector type.
struct UNION_EXPORT SelectorPrivateConcept {
    virtual ~SelectorPrivateConcept();
    virtual int weight() const = 0;
    virtual bool matches(std::shared_ptr<Element> element) const = 0;
    virtual QString toString() const = 0;
    virtual SelectorType type() const = 0;
    virtual bool isCombinator() const = 0;
};

template<SelectorType _type, typename T>
struct SelectorPrivateModel : public SelectorPrivateConcept {
    SelectorPrivateModel(const T &_data)
        : data(_data)
    {
    }

    int weight() const override;
    bool matches(std::shared_ptr<Element> element) const override;
    QString toString() const override;

    inline SelectorType type() const override
    {
        return _type;
    }

    inline bool isCombinator() const override
    {
        return _type == SelectorType::ChildCombinator || _type == SelectorType::DescendantCombinator;
    }

    T data;
};
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
     * Returns the type of selector.
     */
    SelectorType type() const;

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
     * Return whether this selector is a combinator.
     */
    bool isCombinator() const;

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
     * Create a new selector of a specific type, that doesn't need any data.
     */
    template<SelectorType type>
    inline static Selector create([[maybe_unused]] SelectorType _t = type)
    {
        switch (type) {
        case SelectorType::Empty:
            return Selector(nullptr);
        case SelectorType::AnyElement:
        case SelectorType::ChildCombinator:
        case SelectorType::DescendantCombinator:
            return Selector(std::make_shared<detail::SelectorPrivateModel<type, detail::Empty>>(detail::Empty{}));
        default:
            static_assert("Selector type requires data, use create(DataType) instead");
            return Selector(nullptr);
        }
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
    inline static Selector create(DataType &&data)
    {
        return Selector(std::make_shared<detail::SelectorPrivateModel<type, std::decay_t<DataType>>>(std::forward<DataType>(data)));
    }

private:
    Selector(std::shared_ptr<const detail::SelectorPrivateConcept> _d);

    // We want Selector to behave like a value type so that it easy to work with
    // in lists etc. This means we don't want it to have virtual functions, as
    // those would not work when passed by value. We also can't use a template
    // directly, as that would prevent using it in a container. So use type
    // erasure to hide the implementation details is a separate object. Storing
    // that in a shared_ptr we get virtual dispatch to the private details while
    // still allowing copy construction and assignment.
    std::shared_ptr<const detail::SelectorPrivateConcept> d;
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
    /*!
     * Returns a string representation of this list.
     */
    QString toString() const;
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
