// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `generate_properties.py`.

#pragma once

// clang-format off

#include <QDebug>

#include <QImage>

#include "../PropertiesTypes.h"

#include "union_export.h"

// clang-format on

namespace Union
{
namespace Properties
{

class ImagePropertyPrivate;

class UNION_EXPORT ImageProperty
{
public:
    ImageProperty();
    ImageProperty(const ImageProperty &other);
    ImageProperty(ImageProperty &&other);
    ~ImageProperty();

    ImageProperty &operator=(const ImageProperty &other);
    ImageProperty &operator=(ImageProperty &&other);

    std::optional<QImage> imageData() const;
    void setImageData(const std::optional<QImage> &newValue);

    std::optional<qreal> width() const;
    void setWidth(const std::optional<qreal> &newValue);

    std::optional<qreal> height() const;
    void setHeight(const std::optional<qreal> &newValue);

    std::optional<qreal> xOffset() const;
    void setXOffset(const std::optional<qreal> &newValue);

    std::optional<qreal> yOffset() const;
    void setYOffset(const std::optional<qreal> &newValue);

    std::optional<Union::Properties::ImageFlags> flags() const;
    void setFlags(const std::optional<Union::Properties::ImageFlags> &newValue);

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
    static void resolveProperties(const ImageProperty &source, ImageProperty &destination);

private:
    std::unique_ptr<ImagePropertyPrivate> d;
};

UNION_EXPORT bool operator==(const ImageProperty &left, const ImageProperty &right);
UNION_EXPORT inline bool operator!=(const ImageProperty &left, const ImageProperty &right)
{
    return !(left == right);
}

}
}

UNION_EXPORT QDebug operator<<(QDebug debug, const Union::Properties::ImageProperty &type);