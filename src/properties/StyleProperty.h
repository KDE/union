// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
//
// This file is automatically generated from property.h.j2.
// To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off

#include <QDebug>


#include "BackgroundProperty.h"
#include "IconProperty.h"
#include "LayoutProperty.h"
#include "PaletteProperty.h"
#include "TextProperty.h"

#include "union_export.h"

// clang-format on

namespace Union
{
namespace Properties
{

class StylePropertyPrivate;

/*!
\class Union::Properties::StyleProperty
\inmodule core
\ingroup core-properties

\brief The root property group for all elements.

This class contains all the properties that are used to style an element.

*/
class UNION_EXPORT StyleProperty
{
public:
    /*!
     * Default constructor. Constructs a null instance.
     *
     * A null instance in this case means an instance that does not have any
     * values for its properties. This includes property groups.
     */
    StyleProperty();
    /*!
     * Copy constructor.
     */
    StyleProperty(const StyleProperty &other);
    /*!
     * Move constructor.
     */
    StyleProperty(StyleProperty &&other);
    ~StyleProperty();

    /*!
     * Copy assignment operator.
     */
    StyleProperty &operator=(const StyleProperty &other);
    /*!
     * Move assignment operator.
     */
    StyleProperty &operator=(StyleProperty &&other);

    /*!
     * Returns the value of palette.
     */
    std::optional<PaletteProperty> palette() const;
    /*!
     * Set the value of palette.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setPalette(const std::optional<PaletteProperty> &newValue);

    /*!
     * Returns the value of layout.
     */
    std::optional<LayoutProperty> layout() const;
    /*!
     * Set the value of layout.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setLayout(const std::optional<LayoutProperty> &newValue);

    /*!
     * Returns the value of text.
     */
    std::optional<TextProperty> text() const;
    /*!
     * Set the value of text.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setText(const std::optional<TextProperty> &newValue);

    /*!
     * Returns the value of icon.
     */
    std::optional<IconProperty> icon() const;
    /*!
     * Set the value of icon.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setIcon(const std::optional<IconProperty> &newValue);

    /*!
     * Returns the value of background.
     */
    std::optional<BackgroundProperty> background() const;
    /*!
     * Set the value of background.
     *
     * \a newValue The new value or \c{std::nullopt} to unset the value.
     */
    void setBackground(const std::optional<BackgroundProperty> &newValue);

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
    static void resolveProperties(const StyleProperty &source, StyleProperty &destination);

    /*!
     * Create and return an empty StyleProperty instance.
     *
     * This will create an empty StyleProperty instance, which is defined as
     * an instance with all of its values default-constructed. Note that this is
     * different from a default-constructed instance which will have all its
     * values unset.
     */
    static StyleProperty empty();

private:
    std::unique_ptr<StylePropertyPrivate> d;
};

/*!
 * \relates Union::Properties::StyleProperty
 * Equality comparison for StyleProperty.
 */
UNION_EXPORT bool operator==(const StyleProperty &left, const StyleProperty &right);
}
}

/*!
 * \relates Union::Properties::StyleProperty
 * QDebug support for StyleProperty.
 */
UNION_EXPORT QDebug operator<<(QDebug debug, const Union::Properties::StyleProperty &type);