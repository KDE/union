// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "TabWidgetElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

TabWidgetElement::TabWidgetElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_shape(QTabBar::Shape::RoundedNorth)
    , m_isVertical(false)
{
    update(option);
}

TabWidgetElement::~TabWidgetElement()
{
}

void TabWidgetElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    if (const auto newOpt = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(option)) {
        m_tabFrameOption = newOpt;
    }
    if (m_tabFrameOption) {
        m_shape = m_tabFrameOption->shape;
        m_isVertical = m_tabFrameOption->shape == QTabBar::RoundedEast || m_tabFrameOption->shape == QTabBar::RoundedWest
            || m_tabFrameOption->shape == QTabBar::TriangularEast || m_tabFrameOption->shape == QTabBar::TriangularWest;
    }
    layout();
}

void TabWidgetElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }
    switch (enums.PrimitiveElement) {
    case QStyle::PE_FrameTabWidget:
        drawBackground(painter);
        break;
    }
}

Union::IdentifierVariantHash TabWidgetElement::elementAttributes() const
{
    Union::IdentifierVariantHash map;
    const bool top = m_shape == QTabBar::RoundedNorth || m_shape == QTabBar::TriangularNorth;
    const bool bottom = m_shape == QTabBar::RoundedSouth || m_shape == QTabBar::TriangularSouth;
    const bool left = m_shape == QTabBar::RoundedWest || m_shape == QTabBar::TriangularWest;
    const bool right = m_shape == QTabBar::RoundedEast || m_shape == QTabBar::TriangularEast;

    if (top) {
        map[u"direction"_s] = QVariant(u"top"_s);
    }
    if (bottom) {
        map[u"direction"_s] = QVariant(u"bottom"_s);
    }
    if (left) {
        map[u"direction"_s] = QVariant(u"left"_s);
    }
    if (right) {
        map[u"direction"_s] = QVariant(u"right"_s);
    }
    return map;
}

QString TabWidgetElement::fallbackElementName([[maybe_unused]] const QStyleOption *opt) const
{
    return ElementString::TabWidget;
}

QRectF TabWidgetElement::subElementRect(QStyle::SubElement element) const
{
    if (!m_tabFrameOption) {
        return m_styleOption->rect;
    }

    // Do nothing if tabbar is hidden or it's vertical
    if (m_tabFrameOption->tabBarSize.isEmpty() || m_isVertical) {
        return QRect();
    }

    const QRectF rect(m_tabFrameOption->rect);
    QRectF cornerRect;
    switch (element) {
    case QStyle::SE_TabWidgetLeftCorner:
        cornerRect = QRect(QPoint(0, 0), m_tabFrameOption->leftCornerWidgetSize);
        cornerRect.moveLeft(rect.left());
        break;

    case QStyle::SE_TabWidgetRightCorner:
        cornerRect = QRect(QPoint(0, 0), m_tabFrameOption->rightCornerWidgetSize);
        cornerRect.moveRight(rect.right());
        break;

    default:
        break;
    }

    // expend height to tabBarSize, if needed, to make sure base is properly rendered
    cornerRect.setHeight(qMax(cornerRect.height(), m_tabFrameOption->tabBarSize.height() + 1.0));

    switch (m_tabFrameOption->shape) {
    case QTabBar::RoundedNorth:
    case QTabBar::TriangularNorth:
        cornerRect.moveTop(rect.top());
        break;

    case QTabBar::RoundedSouth:
    case QTabBar::TriangularSouth:
        cornerRect.moveBottom(rect.bottom());
        break;

    default:
        break;
    }

    cornerRect = m_style->visualRect(m_styleOption->direction, m_styleOption->rect, cornerRect);
    return cornerRect;
}