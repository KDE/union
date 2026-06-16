// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#pragma once

#include <QPainterPath>
#include <QStyle>
#include <properties/StylePropertyGroup.h>

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

enum class PrimitiveType {
    Standalone,
    Panel,
    Frame,
    Indicator,
};

/*!
 * \brief Draw a Union StylePropertyGroup, such as the center, border and corners.
 */
void drawBackground(QPainter *painter,
                    const QRect &rect,
                    const Union::Properties::StylePropertyGroup *style,
                    const PrimitiveType primitiveType = PrimitiveType::Standalone);

/*!
 * \brief Generates a QPainterPath to use with outlines and rounded corners.
 */
QPainterPath unevenRadiiRectPath(const auto &rect, const Union::Properties::CornersPropertyGroup::CornerRadii cornerRadii);

/*!
 * \brief Constrains the corner radii to not go over the rectangle's size or under zero.
 */
Union::Properties::CornersPropertyGroup::CornerRadii constrainRadii(const QRect &rect, const Union::Properties::CornersPropertyGroup::CornerRadii cornerRadii);

/*!
 * \brief Draw a Union LineProperty.
 */
void drawLineProperty(QPainter *painter,
                      const QRect &rect,
                      SubNodeIndex subNodeIndex,
                      const QMarginsF &borderSizes,
                      const Union::Properties::LinePropertyGroup *line,
                      const Union::Properties::CornersPropertyGroup *corners);

/*!
 * \brief Draw a Union CornerProperty.
 */
void drawCornerProperty(QPainter *painter,
                        const QRect &rect,
                        SubNodeIndex subNodeIndex,
                        const Union::Properties::BorderPropertyGroup *border,
                        const Union::Properties::CornerPropertyGroup *corner);

void drawStyleOption(const QString &elementType, const QStyleOption *opt, QPainter *painter, PrimitiveType primitiveType);