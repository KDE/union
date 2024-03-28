/**
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>
#include <optional>

#include <QColor>
#include <QGradient>
#include <QImage>

#include "Size.h"

namespace Union
{
Q_NAMESPACE

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
    Size xOffset;
    Size yOffset;
    Size width;
    Size height;
};

struct AreaDefinition {
    QColor color = Qt::transparent;
    std::optional<ImageDefinition> image;
    std::optional<QGradient> gradient;

    QSizeF size()
    {
        if (image.has_value()) {
            return QSizeF{image->width.value(), image->height.value()};
        }
        return QSizeF{0.0, 0.0};
    }
};

struct LineDefinition {
    enum class LineStyle {
        Solid,
        Dashed,
    };

    QColor color;
    Size size;
    LineStyle style;
    std::optional<ImageDefinition> image;
};

struct BorderDefinition {
    std::optional<LineDefinition> top;
    std::optional<LineDefinition> bottom;
    std::optional<LineDefinition> left;
    std::optional<LineDefinition> right;
};

struct ShadowEdgeDefinition {
    QColor color;
    Size width;
    std::optional<ImageDefinition> image;
};

struct ShadowDefinition {
    Size xOffset;
    Size yOffset;

    std::optional<ShadowEdgeDefinition> top;
    std::optional<ShadowEdgeDefinition> bottom;
    std::optional<ShadowEdgeDefinition> left;
    std::optional<ShadowEdgeDefinition> right;
};

struct CornerDefinition {
    Size radius;
    Size width;
    Size height;
    std::optional<ImageDefinition> image;
};

struct CornersDefinition {
    std::optional<CornerDefinition> topLeft;
    std::optional<CornerDefinition> topRight;
    std::optional<CornerDefinition> bottomLeft;
    std::optional<CornerDefinition> bottomRight;
};

struct SizeDefinition {
    Size top;
    Size bottom;
    Size left;
    Size right;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Union::ImageFlags)
