// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "MenuBarElement.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

MenuBarElement::MenuBarElement(const QStyleOptionMenuItem *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_menuItemOption(option)
{
    update();
}

MenuBarElement::~MenuBarElement()
{
}

void MenuBarElement::update()
{
    layout();
}

void MenuBarElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_MenuBarEmptyArea:
        drawBackground(painter);
        break;
    }

    switch (enums.PrimitiveElement) {
    case QStyle::PE_PanelMenuBar:
        drawBackground(painter);
        break;
    }
}

qreal MenuBarElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_MenuBarVMargin:
        return averageVPadding();
    case QStyle::PM_MenuBarHMargin:
        return averageHPadding();
    case QStyle::PM_MenuBarPanelWidth:
        return width();
    case QStyle::PM_MenuBarItemSpacing:
        return spacing();
    default:
        break;
    }
    return 0;
}
