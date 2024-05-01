/**
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>
#include <optional>

#include <QColor>
#include <QDebug>
#include <QFont>
#include <QGradient>
#include <QImage>
#include <QMarginsF>

#include "union_export.h"

namespace Union
{
Q_NAMESPACE_EXPORT(UNION_EXPORT)

enum ImageFlag {
    None = 0, //
    RepeatX = 1 << 0, //
    RepeatY = 1 << 1, //
    RepeatBoth = RepeatX | RepeatY, //
    StretchX = 1 << 2, //
    StretchY = 1 << 3, //
    StretchBoth = StretchX | StretchY, //
};
Q_ENUM_NS(ImageFlag)
Q_DECLARE_FLAGS(ImageFlags, ImageFlag)

struct ImageDefinition {
    QImage imageData;
    ImageFlags flags = ImageFlag::StretchBoth;
    qreal xOffset = 0.0;
    qreal yOffset = 0.0;
    qreal width = 0.0;
    qreal height = 0.0;

    inline bool operator==(const ImageDefinition &other) const
    {
        return imageData == other.imageData && flags == other.flags;
    }
};

inline bool operator==(const std::optional<ImageDefinition> &first, const std::optional<ImageDefinition> &second)
{
    if (first.has_value()) {
        if (second.has_value()) {
            return *first == *second;
        }
    } else {
        if (!second.has_value()) {
            return true;
        }
    }
    return false;
}

struct SizeDefinition {
    qreal left = 0.0;
    qreal right = 0.0;
    qreal top = 0.0;
    qreal bottom = 0.0;

    QMarginsF toMargins() const
    {
        return QMarginsF(left, top, right, bottom);
    }
};

struct AreaDefinition {
    QSizeF size;
    QColor color = Qt::transparent;
    std::optional<ImageDefinition> image;
};

struct LineDefinition {
    enum class LineStyle {
        Solid,
        Dashed,
    };

    QColor color = Qt::transparent;
    qreal size = 0.0;
    LineStyle style = LineStyle::Solid;
    std::optional<ImageDefinition> image;

    inline bool operator==(const LineDefinition &other) const
    {
        return color == other.color && qFuzzyCompare(size, other.size) && style == other.style && image == other.image;
    }
};

struct CornerDefinition {
    qreal radius = 0.0;
    qreal width = 0.0;
    qreal height = 0.0;
    std::optional<ImageDefinition> image;

    inline bool operator==(const CornerDefinition &other) const
    {
        return qFuzzyCompare(radius, other.radius) && qFuzzyCompare(width, other.width) && qFuzzyCompare(height, other.height) && image == other.image;
    }
};

struct BorderDefinition {
    std::optional<LineDefinition> left;
    std::optional<LineDefinition> right;
    std::optional<LineDefinition> top;
    std::optional<LineDefinition> bottom;
};

struct ShadowDefinition {
    std::optional<SizeDefinition> offsets;

    std::optional<LineDefinition> left;
    std::optional<LineDefinition> right;
    std::optional<LineDefinition> top;
    std::optional<LineDefinition> bottom;

    std::optional<CornerDefinition> topLeft;
    std::optional<CornerDefinition> topRight;
    std::optional<CornerDefinition> bottomLeft;
    std::optional<CornerDefinition> bottomRight;
};

struct CornersDefinition {
    std::optional<CornerDefinition> topLeft;
    std::optional<CornerDefinition> topRight;
    std::optional<CornerDefinition> bottomLeft;
    std::optional<CornerDefinition> bottomRight;
};

struct TextDefinition {
    Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignTop;
    QFont font;
};
//
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Union::ImageFlags)

UNION_EXPORT inline QDebug operator<<(QDebug debug, const Union::ImageDefinition &image)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "ImageDefinition(data=" << image.imageData << ", flags=" << image.flags << ")";
    return debug;
}

UNION_EXPORT inline QDebug operator<<(QDebug debug, const Union::SizeDefinition &size)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "SizeDefinition(left=" << size.left << ", right=" << size.right << ", top=" << size.top << ", bottom=" << size.bottom << ")";
    return debug;
}

UNION_EXPORT inline QDebug operator<<(QDebug debug, const Union::AreaDefinition &area)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "AreaDefinition(size=" << area.size << ", color=" << area.color << ", image=" << area.image << ")";
    return debug;
}
