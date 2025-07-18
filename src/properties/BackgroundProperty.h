// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
//
// This file is automatically generated from property.h.j2.
// To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off

#include <QDebug>

#include <QColor>

#include "ImageProperty.h"

#include "union_export.h"

// clang-format on

namespace Union
{
namespace Properties
{

class BackgroundPropertyPrivate;

/*!
\class Union::Properties::BackgroundProperty
\inmodule core
\ingroup core-properties

\brief A property group containing properties related to an element's background.

*/
class UNION_EXPORT BackgroundProperty
{
public:
    /*!
     * Default constructor. Constructs a null instance.
     *
     * A null instance in this case means an instance that does not have any
     * values for its properties. This includes property groups.
     */
    BackgroundProperty();
    /*!
     * Copy constructor.
     */
    BackgroundProperty(const BackgroundProperty &other);
    /*!
     * Move constructor.
     */
    BackgroundProperty(BackgroundProperty &&other);
    ~BackgroundProperty();

    /*!
     * Copy assignment operator.
     */
    BackgroundProperty &operator=(const BackgroundProperty &other);
    /*!
     * Move assignment operator.
     */
    BackgroundProperty &operator=(BackgroundProperty &&other);

    /*!
     * Returns the value of color.
     */
    std::optional<QColor> color() const;
    /*!
     * Set the value of color.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setColor(const std::optional<QColor> &newValue);

    /*!
     * Returns the value of image.
     */
    std::optional<ImageProperty> image() const;
    /*!
     * Returns image if set or a new ImageProperty if not.
     */
    ImageProperty image_or_new() const;
    /*!
     * Set the value of image.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setImage(const std::optional<ImageProperty> &newValue);

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
    static void resolveProperties(const BackgroundProperty &source, BackgroundProperty &destination);

    /*!
     * Create and return an empty BackgroundProperty instance.
     *
     * This will create an empty BackgroundProperty instance, which is defined as
     * an instance with all of its values default-constructed. Note that this is
     * different from a default-constructed instance which will have all its
     * values unset.
     */
    static BackgroundProperty empty();

private:
    std::unique_ptr<BackgroundPropertyPrivate> d;
};

/*!
 * \relates Union::Properties::BackgroundProperty
 * Equality comparison for BackgroundProperty.
 */
UNION_EXPORT bool operator==(const BackgroundProperty &left, const BackgroundProperty &right);
}
}

/*!
 * \relates Union::Properties::BackgroundProperty
 * QDebug support for BackgroundProperty.
 */
UNION_EXPORT QDebug operator<<(QDebug debug, const Union::Properties::BackgroundProperty &type);