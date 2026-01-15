// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#pragma once

#include <properties/StyleProperty.h>

class QPainter;

enum class SubNodeIndex {
    Left,
    Right,
    Top,
    Bottom,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
};

/*!
 * \brief Draw a Union StyleProperty, such as the center, border and corners.
 */
void drawBackground(QPainter *painter, const QRect &rect, const Union::Properties::StyleProperty *style);

/*!
 * \brief Draw a Union LineProperty.
 */
void drawLineProperty(QPainter *painter,
                      const QRect &rect,
                      SubNodeIndex subNodeIndex,
                      const QMarginsF &borderSizes,
                      const Union::Properties::LineProperty *line,
                      const Union::Properties::CornersProperty *corners);

/*!
 * \brief Draw a Union CornerProperty.
 */
void drawCornerProperty(QPainter *painter,
                        const QRect &rect,
                        SubNodeIndex subNodeIndex,
                        const QMarginsF &borderSizes,
                        const Union::Properties::CornerProperty *corner);
