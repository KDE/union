// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
//
// This file is automatically generated from property.h.j2.
// To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off

#include <QDebug>


#include "CornerProperty.h"

#include "union_export.h"

// clang-format on

namespace Union
{
namespace Properties
{

class CornersPropertyPrivate;

/*!
\class Union::Properties::CornersProperty
\inmodule core
\ingroup core-properties

\brief A property group containing properties of a background's corners.

*/
class UNION_EXPORT CornersProperty
{
public:
    /*!
     * Default constructor. Constructs a null instance.
     *
     * A null instance in this case means an instance that does not have any
     * values for its properties. This includes property groups.
     */
    CornersProperty();
    /*!
     * Copy constructor.
     */
    CornersProperty(const CornersProperty &other);
    /*!
     * Move constructor.
     */
    CornersProperty(CornersProperty &&other);
    ~CornersProperty();

    /*!
     * Copy assignment operator.
     */
    CornersProperty &operator=(const CornersProperty &other);
    /*!
     * Move assignment operator.
     */
    CornersProperty &operator=(CornersProperty &&other);

    /*!
     * Returns the value of topLeft.
     */
    std::optional<CornerProperty> topLeft() const;
    /*!
     * Returns topLeft if set or a new CornerProperty if not.
     */
    CornerProperty topLeft_or_new() const;
    /*!
     * Set the value of topLeft.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setTopLeft(const std::optional<CornerProperty> &newValue);

    /*!
     * Returns the value of topRight.
     */
    std::optional<CornerProperty> topRight() const;
    /*!
     * Returns topRight if set or a new CornerProperty if not.
     */
    CornerProperty topRight_or_new() const;
    /*!
     * Set the value of topRight.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setTopRight(const std::optional<CornerProperty> &newValue);

    /*!
     * Returns the value of bottomLeft.
     */
    std::optional<CornerProperty> bottomLeft() const;
    /*!
     * Returns bottomLeft if set or a new CornerProperty if not.
     */
    CornerProperty bottomLeft_or_new() const;
    /*!
     * Set the value of bottomLeft.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setBottomLeft(const std::optional<CornerProperty> &newValue);

    /*!
     * Returns the value of bottomRight.
     */
    std::optional<CornerProperty> bottomRight() const;
    /*!
     * Returns bottomRight if set or a new CornerProperty if not.
     */
    CornerProperty bottomRight_or_new() const;
    /*!
     * Set the value of bottomRight.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setBottomRight(const std::optional<CornerProperty> &newValue);

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
    static void resolveProperties(const CornersProperty &source, CornersProperty &destination);

    /*!
     * Create and return an empty CornersProperty instance.
     *
     * This will create an empty CornersProperty instance, which is defined as
     * an instance with all of its values default-constructed. Note that this is
     * different from a default-constructed instance which will have all its
     * values unset.
     */
    static CornersProperty empty();

private:
    std::unique_ptr<CornersPropertyPrivate> d;
};

/*!
 * \relates Union::Properties::CornersProperty
 * Equality comparison for CornersProperty.
 */
UNION_EXPORT bool operator==(const CornersProperty &left, const CornersProperty &right);
}
}

/*!
 * \relates Union::Properties::CornersProperty
 * QDebug support for CornersProperty.
 */
UNION_EXPORT QDebug operator<<(QDebug debug, const Union::Properties::CornersProperty &type);