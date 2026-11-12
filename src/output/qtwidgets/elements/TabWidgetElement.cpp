// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "TabWidgetElement.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

TabWidgetElement::TabWidgetElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_shape(QTabBar::Shape::RoundedNorth)
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
