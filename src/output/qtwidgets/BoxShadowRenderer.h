/*
 * SPDX-FileCopyrightText: 2018 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include "MenuShadowHelper.h"
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QSize>

class BoxShadowRenderer
{
public:
    /**
     * Set the size of the box.
     * @param size The size of the box.
     **/
    void setBoxSize(const QSizeF &size);

    /**
     * Add a shadow.
     * @param Shadow The shadow struct.
     **/
    void addShadow(const Shadow &shadow);

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
    static QSize calculateMinimumBoxSize(const Shadow &shadow);

    /**
     * Calculate the minimum size of the shadow texture.
     *
     * This helper computes the minimum size of the resulting texture so the shadow
     * is not clipped.
     *
     * @param boxSize The size of the box.
     * @param shadow The shadow struct.
     **/
    static QSizeF calculateMinimumShadowTextureSize(const QSizeF &boxSize, const Shadow &shadow);

private:
    QSizeF m_boxSize;

    QVector<Shadow> m_shadows;
};
