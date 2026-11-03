// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#pragma once

#include <QPainterPath>
#include <QStyle>
#include <properties/StylePropertyGroup.h>

class QPainter;

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

    UnionPainter(QPainter *painter, const Union::StylePropertyGroupPtr &properties, const QStyleOption *option = nullptr, const QWidget *widget = nullptr);
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
    const QStyleOption *m_option;
    const QWidget *m_widget;

    Parts m_drawParts;
    QRectF m_drawRect;
    QColor m_overrideColor;

    template<typename F>
    Borders borderFromProperties(F getter)
    {
        using BorderType = std::remove_pointer_t<std::invoke_result_t<F, Union::Properties::StylePropertyGroup *>>;
        Borders borders;
        borders.top.size = safePropertyLookup(m_properties, 0.0, getter, &BorderType::top, &Union::Properties::LinePropertyGroup::size);
        borders.left.size = safePropertyLookup(m_properties, 0.0, getter, &BorderType::left, &Union::Properties::LinePropertyGroup::LinePropertyGroup::size);
        borders.bottom.size =
            safePropertyLookup(m_properties, 0.0, getter, &BorderType::bottom, &Union::Properties::LinePropertyGroup::LinePropertyGroup::size);
        borders.right.size = safePropertyLookup(m_properties, 0.0, getter, &BorderType::right, &Union::Properties::LinePropertyGroup::LinePropertyGroup::size);
        borders.top.color =
            safePropertyLookup(m_properties, Union::Color(), getter, &BorderType::top, &Union::Properties::LinePropertyGroup::LinePropertyGroup::color)
                .toQColor();
        borders.left.color =
            safePropertyLookup(m_properties, Union::Color(), getter, &BorderType::left, &Union::Properties::LinePropertyGroup::LinePropertyGroup::color)
                .toQColor();
        borders.bottom.color =
            safePropertyLookup(m_properties, Union::Color(), getter, &BorderType::bottom, &Union::Properties::LinePropertyGroup::LinePropertyGroup::color)
                .toQColor();
        borders.right.color =
            safePropertyLookup(m_properties, Union::Color(), getter, &BorderType::right, &Union::Properties::LinePropertyGroup::LinePropertyGroup::color)
                .toQColor();
        return borders;
    }
};
