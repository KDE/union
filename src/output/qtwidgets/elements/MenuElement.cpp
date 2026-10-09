// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "MenuElement.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

#include "ShadowRenderer.h"
#include "SharedNames.h"

using namespace Qt::StringLiterals;

MenuElement::MenuElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_menuOption(option)
{
    update();
    if (m_elementProperties && m_menuOption) {
        m_shadowRenderer = std::make_unique<ShadowRenderer>(m_elementProperties);
        m_shadowWidget = qobject_cast<QWidget *>(m_menuOption->styleObject);
    }
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
        if (m_shadowRenderer && m_shadowWidget) {
            m_shadowRenderer->drawShadow(m_shadowWidget);
        }
        break;
    case QStyle::PE_FrameMenu:
        drawFrame(painter);
        if (m_shadowRenderer && m_shadowWidget) {
            m_shadowRenderer->drawShadow(m_shadowWidget);
        }
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
    return contentsSizeFromStyle;
}

qreal MenuElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_MenuVMargin:
        return averageVPadding();
    case QStyle::PM_MenuHMargin:
        return averageHPadding();
    case QStyle::PM_MenuPanelWidth: // This is actually border width according docs
        return averageBorderSize();
    case QStyle::PM_MenuDesktopFrameWidth:
        return 0; // Fusion also returns 0, likely unused
    default:
        break;
    }
    return 0;
}

QString MenuElement::fallbackElementName([[maybe_unused]] const QStyleOption *opt) const
{
    return ElementString::Menu;
}
