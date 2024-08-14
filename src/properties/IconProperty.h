// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off

#include <QDebug>

#include <QColor>
#include <QString>
#include <QUrl>


#include "union_export.h"

// clang-format on

namespace Union
{
namespace Properties
{

class IconPropertyPrivate;

class UNION_EXPORT IconProperty
{
public:
    IconProperty();
    IconProperty(const IconProperty &other);
    IconProperty(IconProperty &&other);
    ~IconProperty();

    IconProperty &operator=(const IconProperty &other);
    IconProperty &operator=(IconProperty &&other);

    std::optional<qreal> width() const;
    void setWidth(const std::optional<qreal> &newValue);

    std::optional<qreal> height() const;
    void setHeight(const std::optional<qreal> &newValue);

    std::optional<QColor> color() const;
    void setColor(const std::optional<QColor> &newValue);

    std::optional<QString> name() const;
    void setName(const std::optional<QString> &newValue);

    std::optional<QUrl> source() const;
    void setSource(const std::optional<QUrl> &newValue);

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
    static void resolveProperties(const IconProperty &source, IconProperty &destination);

private:
    std::unique_ptr<IconPropertyPrivate> d;
};

UNION_EXPORT bool operator==(const IconProperty &left, const IconProperty &right);
UNION_EXPORT inline bool operator!=(const IconProperty &left, const IconProperty &right)
{
    return !(left == right);
}

}
}

UNION_EXPORT QDebug operator<<(QDebug debug, const Union::Properties::IconProperty &type);