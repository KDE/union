// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#pragma once

#include <QPainterPath>
#include <QStyle>
#include <properties/StylePropertyGroup.h>

class QPainter;

namespace Union
{
namespace Widgets
{

struct Line {
    qreal size = 0.0;
    QColor color;
};
struct Borders {
    Union::Properties::CornersPropertyGroup::CornerRadii cornerRadii;
    Line top;
    Line left;
    Line bottom;
    Line right;
};

class UnionPainter : public QObject
{
    Q_OBJECT
public:
    enum class Parts {
        All,
        PanelOnly,
        FrameOnly,
        Outline
    };
    Q_ENUM(Parts)

    UnionPainter(QPainter *painter, const Union::StylePropertyGroupPtr &properties);
    ~UnionPainter() override;

    /*!
     * \brief Draw a Union StylePropertyGroup, such as the center, border and corners.
     */
    void draw();

    /*!
     * \brief Generates a QPainterPath to use with outlines and rounded corners.
     */
    static QPainterPath unevenRadiiRectPath(const QRectF &rect, const Union::Properties::CornersPropertyGroup::CornerRadii cornerRadii);

    void setParts(Parts parts);
    void setOverrideBackgroundColor(const QColor &color);
    void setRect(const QRectF &rect);

private:
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
     * \brief Constrains the corner radii to not go over the rectangle's size or under zero.
     */
    Union::Properties::CornersPropertyGroup::CornerRadii constrainRadii(const QRectF &rect,
                                                                        const Union::Properties::CornersPropertyGroup::CornerRadii cornerRadii);

    /*!
     * \brief Draw a Union LineProperty.
     */
    void drawLineProperty(const QRectF &rect, SubNodeIndex subNodeIndex, const Borders &borders);

    /*!
     * \brief Draw a Union CornerProperty.
     */
    void drawCornerProperty(const QRectF &rect, SubNodeIndex subNodeIndex, const Borders &borders);

    QPainter *m_painter;
    Union::StylePropertyGroupPtr m_properties;

    Parts m_drawParts = Parts::All;
    QRectF m_drawRect;
    QColor m_overrideColor;
};
}
}
