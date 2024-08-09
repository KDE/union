// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `generate_properties.py`.

#pragma once

// clang-format off

#include <QDebug>

#include <QColor>

#include "../PropertiesTypes.h"
#include "ImageProperty.h"

#include "union_export.h"

// clang-format on

namespace Union
{
namespace Properties
{

class LinePropertyPrivate;

class UNION_EXPORT LineProperty
{
public:
    LineProperty();
    LineProperty(const LineProperty &other);
    LineProperty(LineProperty &&other);
    ~LineProperty();

    LineProperty &operator=(const LineProperty &other);
    LineProperty &operator=(LineProperty &&other);

    std::optional<qreal> size() const;
    void setSize(const std::optional<qreal> &newValue);

    std::optional<QColor> color() const;
    void setColor(const std::optional<QColor> &newValue);

    std::optional<Union::Properties::LineStyle> style() const;
    void setStyle(const std::optional<Union::Properties::LineStyle> &newValue);

    std::optional<ImageProperty> image() const;
    void setImage(const std::optional<ImageProperty> &newValue);

    /**
     * Check if this property has any value set.
     *
     * Note that for any sub property that this property has, it also checks if
     * that sub-property has any value.
     */
    bool hasAnyValue() const;

    /**
     * Copy values from source to destination if destination does not have a value.
     *
     * This will recursively copy sub-values.
     *
     * \param source The source property to copy from.
     * \param destination The destination property to copy to.
     */
    static void resolveProperties(const LineProperty &source, LineProperty &destination);

private:
    std::unique_ptr<LinePropertyPrivate> d;
};

UNION_EXPORT bool operator==(const LineProperty &left, const LineProperty &right);
UNION_EXPORT inline bool operator!=(const LineProperty &left, const LineProperty &right)
{
    return !(left == right);
}

}
}

UNION_EXPORT QDebug operator<<(QDebug debug, const Union::Properties::LineProperty &type);