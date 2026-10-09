// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include <QPixmap>
#include <QRect>
#include <QVector>

namespace Union
{

class ShadowTileMap
{
public:
    enum Position {
        TopLeft,
        TopCenter,
        TopRight,
        MiddleLeft,
        MiddleCenter,
        MiddleRight,
        BottomLeft,
        BottomCenter,
        BottomRight
    };

    ShadowTileMap(const QPixmap &texture, const QSizeF &topLeftSize, const QSizeF &middleSize);

    ShadowTileMap();

    virtual ~ShadowTileMap()
    {
    }

    void draw(const QRectF &drawRect, QPainter *painter) const;

    bool isValid() const;

    QVector<QPixmap> pixmaps() const;

private:
    void initializeTile(const QSizeF &size, const QRectF &targetRect);

    QPixmap m_texture;
    QVector<QPixmap> m_pixmaps;
    QSizeF m_topLeft;
    QSizeF m_middle;
    QSizeF m_bottomRight;

    static constexpr int tileMapSize = 9;
};
}
