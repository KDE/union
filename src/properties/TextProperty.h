// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
//
// This file is automatically generated from property.h.j2.
// To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off

#include <QDebug>

#include <QFont>

#include "AlignmentProperty.h"

#include "union_export.h"

// clang-format on

namespace Union
{
namespace Properties
{

class TextPropertyPrivate;

/*!
\class Union::Properties::TextProperty
\inmodule core
\ingroup core-properties

\brief A property group containing properties related to the text of an element.

*/
class UNION_EXPORT TextProperty
{
public:
    /*!
     * Default constructor. Constructs a null instance.
     *
     * A null instance in this case means an instance that does not have any
     * values for its properties. This includes property groups.
     */
    TextProperty();
    /*!
     * Copy constructor.
     */
    TextProperty(const TextProperty &other);
    /*!
     * Move constructor.
     */
    TextProperty(TextProperty &&other);
    ~TextProperty();

    /*!
     * Copy assignment operator.
     */
    TextProperty &operator=(const TextProperty &other);
    /*!
     * Move assignment operator.
     */
    TextProperty &operator=(TextProperty &&other);

    /*!
     * Returns the value of alignment.
     */
    std::optional<AlignmentProperty> alignment() const;
    /*!
     * Returns alignment if set or a new AlignmentProperty if not.
     */
    AlignmentProperty alignment_or_new() const;
    /*!
     * Set the value of alignment.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setAlignment(const std::optional<AlignmentProperty> &newValue);

    /*!
     * Returns the value of font.
     */
    std::optional<QFont> font() const;
    /*!
     * Set the value of font.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setFont(const std::optional<QFont> &newValue);

    /*!
     * Returns if this property group has any value set.
     *
     * Note that for any property that is also a property group, this will also
     * check if that group has any value.
     */
    bool hasAnyValue() const;

    /*!
     * Copy property values from source to destination if destination does not have a property value.
     *
     * This will recursively copy property values of grouped properties.
     *
     * \a source      The source property group to copy from.
     * \a destination The destination property group to copy to.
     */
    static void resolveProperties(const TextProperty &source, TextProperty &destination);

    /*!
     * Create and return an empty TextProperty instance.
     *
     * This will create an empty TextProperty instance, which is defined as
     * an instance with all of its values default-constructed. Note that this is
     * different from a default-constructed instance which will have all its
     * values unset.
     */
    static TextProperty empty();

private:
    std::unique_ptr<TextPropertyPrivate> d;
};

/*!
 * \relates Union::Properties::TextProperty
 * Equality comparison for TextProperty.
 */
UNION_EXPORT bool operator==(const TextProperty &left, const TextProperty &right);
}
}

/*!
 * \relates Union::Properties::TextProperty
 * QDebug support for TextProperty.
 */
UNION_EXPORT QDebug operator<<(QDebug debug, const Union::Properties::TextProperty &type);