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

TabWidgetElement::TabWidgetElement(const QStyleOptionTabWidgetFrame *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_tabFrameOption(option)
{
    update();
}

TabWidgetElement::~TabWidgetElement()
{
}

void TabWidgetElement::update()
{
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

QVariantMap TabWidgetElement::elementAttributes() const
{
    QVariantMap map;
    const bool top = m_tabFrameOption->shape == QTabBar::RoundedNorth || m_tabFrameOption->shape == QTabBar::TriangularNorth;
    const bool bottom = m_tabFrameOption->shape == QTabBar::RoundedSouth || m_tabFrameOption->shape == QTabBar::TriangularSouth;
    const bool left = m_tabFrameOption->shape == QTabBar::RoundedWest || m_tabFrameOption->shape == QTabBar::TriangularWest;
    const bool right = m_tabFrameOption->shape == QTabBar::RoundedEast || m_tabFrameOption->shape == QTabBar::TriangularEast;

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

QStringList TabWidgetElement::elementHints() const
{
    return QStringList();
}
