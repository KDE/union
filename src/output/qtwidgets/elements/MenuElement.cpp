// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "MenuElement.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

#include "SharedNames.h"

using namespace Qt::StringLiterals;

MenuElement::MenuElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_menuOption(option)
{
    update();
}

MenuElement::~MenuElement()
{
}

void MenuElement::update()
{
    updateSubElementList();
    layout();
}

void MenuElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }
    switch (enums.PrimitiveElement) {
    case QStyle::PE_PanelMenu:
        drawBackground(painter);
        break;
    case QStyle::PE_FrameMenu:
        drawFrame(painter);
        break;
    }
}

void MenuElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(ElementString::Frame);
}

QSizeF MenuElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    return applyPaddingToSize(contentsSizeFromStyle);
}

qreal MenuElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_MenuVMargin:
        return averageVPadding();
    case QStyle::PM_MenuHMargin:
        return averageHPadding();
    case QStyle::PM_MenuPanelWidth:
        return width();
    case QStyle::PM_MenuDesktopFrameWidth:
        return averageBorderSize();
    default:
        break;
    }
    return 0;
}
