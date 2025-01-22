// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
//
// This file is automatically generated from property.h.j2.
// To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off

#include <QDebug>


#include "AlignmentProperty.h"
#include "SizeProperty.h"

#include "union_export.h"

// clang-format on

namespace Union
{
namespace Properties
{

class LayoutPropertyPrivate;

/*!
\class Union::Properties::LayoutProperty
\inmodule core
\ingroup core-properties

\brief A property group containing properties related to the layout of an element.

*/
class UNION_EXPORT LayoutProperty
{
public:
    /*!
     * Default constructor. Constructs a null instance.
     *
     * A null instance in this case means an instance that does not have any
     * values for its properties. This includes property groups.
     */
    LayoutProperty();
    /*!
     * Copy constructor.
     */
    LayoutProperty(const LayoutProperty &other);
    /*!
     * Move constructor.
     */
    LayoutProperty(LayoutProperty &&other);
    ~LayoutProperty();

    /*!
     * Copy assignment operator.
     */
    LayoutProperty &operator=(const LayoutProperty &other);
    /*!
     * Move assignment operator.
     */
    LayoutProperty &operator=(LayoutProperty &&other);

    /*!
     * Returns the value of alignment.
     */
    std::optional<AlignmentProperty> alignment() const;
    /*!
     * Set the value of alignment.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setAlignment(const std::optional<AlignmentProperty> &newValue);

    /*!
     * Returns the value of width.
     */
    std::optional<qreal> width() const;
    /*!
     * Set the value of width.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setWidth(const std::optional<qreal> &newValue);

    /*!
     * Returns the value of height.
     */
    std::optional<qreal> height() const;
    /*!
     * Set the value of height.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setHeight(const std::optional<qreal> &newValue);

    /*!
     * Returns the value of spacing.
     */
    std::optional<qreal> spacing() const;
    /*!
     * Set the value of spacing.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setSpacing(const std::optional<qreal> &newValue);

    /*!
     * Returns the value of padding.
     */
    std::optional<SizeProperty> padding() const;
    /*!
     * Set the value of padding.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setPadding(const std::optional<SizeProperty> &newValue);

    /*!
     * Returns the value of inset.
     */
    std::optional<SizeProperty> inset() const;
    /*!
     * Set the value of inset.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setInset(const std::optional<SizeProperty> &newValue);

    /*!
     * Returns the value of margins.
     */
    std::optional<SizeProperty> margins() const;
    /*!
     * Set the value of margins.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setMargins(const std::optional<SizeProperty> &newValue);

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
    static void resolveProperties(const LayoutProperty &source, LayoutProperty &destination);

    /*!
     * Create and return an empty LayoutProperty instance.
     *
     * This will create an empty LayoutProperty instance, which is defined as
     * an instance with all of its values default-constructed. Note that this is
     * different from a default-constructed instance which will have all its
     * values unset.
     */
    static LayoutProperty empty();

private:
    std::unique_ptr<LayoutPropertyPrivate> d;
};

/*!
 * \relates Union::Properties::LayoutProperty
 * Equality comparison for LayoutProperty.
 */
UNION_EXPORT bool operator==(const LayoutProperty &left, const LayoutProperty &right);
}
}

/*!
 * \relates Union::Properties::LayoutProperty
 * QDebug support for LayoutProperty.
 */
UNION_EXPORT QDebug operator<<(QDebug debug, const Union::Properties::LayoutProperty &type);