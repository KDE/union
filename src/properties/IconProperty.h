// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
//
// This file is automatically generated from property.h.j2.
// To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off

#include <QDebug>

#include <QString>
#include <QUrl>

#include "AlignmentProperty.h"

#include "union_export.h"

// clang-format on

namespace Union
{
namespace Properties
{

class IconPropertyPrivate;

/*!
\class Union::Properties::IconProperty
\inmodule core
\ingroup core-properties

\brief A property group containing properties related to an element's icon.

*/
class UNION_EXPORT IconProperty
{
public:
    /*!
     * Default constructor. Constructs a null instance.
     *
     * A null instance in this case means an instance that does not have any
     * values for its properties. This includes property groups.
     */
    IconProperty();
    /*!
     * Copy constructor.
     */
    IconProperty(const IconProperty &other);
    /*!
     * Move constructor.
     */
    IconProperty(IconProperty &&other);
    ~IconProperty();

    /*!
     * Copy assignment operator.
     */
    IconProperty &operator=(const IconProperty &other);
    /*!
     * Move assignment operator.
     */
    IconProperty &operator=(IconProperty &&other);

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
     * Returns the value of name.
     */
    std::optional<QString> name() const;
    /*!
     * Set the value of name.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setName(const std::optional<QString> &newValue);

    /*!
     * Returns the value of source.
     */
    std::optional<QUrl> source() const;
    /*!
     * Set the value of source.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setSource(const std::optional<QUrl> &newValue);

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
    static void resolveProperties(const IconProperty &source, IconProperty &destination);

    /*!
     * Create and return an empty IconProperty instance.
     *
     * This will create an empty IconProperty instance, which is defined as
     * an instance with all of its values default-constructed. Note that this is
     * different from a default-constructed instance which will have all its
     * values unset.
     */
    static IconProperty empty();

private:
    std::unique_ptr<IconPropertyPrivate> d;
};

/*!
 * \relates Union::Properties::IconProperty
 * Equality comparison for IconProperty.
 */
UNION_EXPORT bool operator==(const IconProperty &left, const IconProperty &right);
}
}

/*!
 * \relates Union::Properties::IconProperty
 * QDebug support for IconProperty.
 */
UNION_EXPORT QDebug operator<<(QDebug debug, const Union::Properties::IconProperty &type);