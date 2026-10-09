/*
 * SPDX-FileCopyrightText: 2018 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include "properties/CornersPropertyGroup.h"
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QSize>

class BoxShadowRenderer
{
public:
    // Adjust the shadow so that its slightly under the widget to
    // avoid any gaps. Take this into account when drawing the shadow.
    static constexpr int SHADOW_ADJUSTMENT = 2;

    struct Shadow {
        Shadow() = default;

        Shadow(const QPoint &offset, qreal blur, qreal spread, const QColor &color)
            : offset(offset)
            , blur(blur)
            , spread(spread)
            , color(color)
        {
        }

        QPoint offset;
        qreal blur;
        qreal spread;
        QColor color;
        Union::Properties::CornersPropertyGroup::CornerRadii cornerRadius;

        bool isNone() const
        {
            return (blur == 0 && spread == 0);
        }
    };

    /**
     * Set the size of the box.
     * @param size The size of the box.
     **/
    void setBoxSize(const QSizeF &size);

    /**
     * Add a shadow.
     * @param Shadow The shadow struct.
     **/
    void addShadow(const BoxShadowRenderer::Shadow &shadow);

    /**
     * Render the shadow.
     **/
    QImage render() const;

    /**
     * Calculate the minimum size of the box.
     *
     * This helper computes the minimum size of the box so the shadow behind it has
     * full its strength.
     *
     * @param radius The blur radius of the shadow.
     **/
    static QSize calculateMinimumBoxSize(const BoxShadowRenderer::Shadow &shadow);

    /**
     * Calculate the minimum size of the shadow texture.
     *
     * This helper computes the minimum size of the resulting texture so the shadow
     * is not clipped.
     *
     * @param boxSize The size of the box.
     * @param shadow The shadow struct.
     **/
    static QSizeF calculateMinimumShadowTextureSize(const QSizeF &boxSize, const BoxShadowRenderer::Shadow &shadow);

private:
    QSizeF m_boxSize;

    QVector<BoxShadowRenderer::Shadow> m_shadows;
};
