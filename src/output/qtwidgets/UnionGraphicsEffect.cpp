// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "UnionGraphicsEffect.h"
#include "UnionPainter.h"
#include <QPainter>

using namespace Union::Properties;
using namespace Union::Widgets;

UnionGraphicsEffect::UnionGraphicsEffect(QObject *parent)
    : QGraphicsDropShadowEffect(parent)
    , m_isValid(false)
{
    setEnabled(false);
}

UnionGraphicsEffect::~UnionGraphicsEffect()
{
}

void UnionGraphicsEffect::draw(QPainter *painter)
{
    if (m_isValid) {
        // Draw shadow first with QGraphicsDropShadowEffect
        QGraphicsDropShadowEffect::draw(painter);

        // Now draw outline
        UnionPainter p(painter, m_properties);
        // Use the drawRect with the outline margins so we draw the outline to correct position
        p.setRect(m_drawRect.marginsAdded(m_outlineSize.toMargins()));
        p.setParts(UnionPainter::Parts::Outline);
        p.draw();

    } else {
        drawSource(painter);
    }
}

void UnionGraphicsEffect::setProperties(Union::StylePropertyGroupPtr properties)
{
    if (properties) {
        m_properties = properties;
        auto shadowColor = safePropertyLookup(m_properties, Union::Color(), &StylePropertyGroup::shadow, &ShadowPropertyGroup::color);
        auto shadowOffset =
            safePropertyLookup(m_properties, QVector2D(), &StylePropertyGroup::shadow, &ShadowPropertyGroup::offset, &OffsetPropertyGroup::toVector2D);
        auto shadowBlur = safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::shadow, &ShadowPropertyGroup::blur);
        setColor(shadowColor.toQColor());
        setBlurRadius(shadowBlur);
        setOffset(shadowOffset.x(), shadowOffset.y());

        m_outlineSize.setTop(safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::outline, &OutlinePropertyGroup::top, &LinePropertyGroup::size));
        m_outlineSize.setLeft(safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::outline, &OutlinePropertyGroup::left, &LinePropertyGroup::size));
        m_outlineSize.setBottom(safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::outline, &OutlinePropertyGroup::bottom, &LinePropertyGroup::size));
        m_outlineSize.setRight(safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::outline, &OutlinePropertyGroup::right, &LinePropertyGroup::size));

        // If we dont draw shadow, ensure we still give room for the outline
        if (blurRadius() <= 0) {
            setBlurRadius(std::max({m_outlineSize.top(), m_outlineSize.bottom(), m_outlineSize.right(), m_outlineSize.left()}));
        }

        m_isValid = true;
        setEnabled(true);
    }
}

void UnionGraphicsEffect::setDrawRect(const QRectF &rect)
{
    m_drawRect = rect;
}

QRectF UnionGraphicsEffect::boundingRectFor(const QRectF &rect) const
{
    // Ensure our bounding rect has enough space to draw both outline and shadow
    auto newRect = QGraphicsDropShadowEffect::boundingRectFor(rect);
    return newRect.marginsAdded(m_outlineSize);
}