// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "TabCloseButtonElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

TabCloseButtonElement::TabCloseButtonElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
{
    update(option);
}

TabCloseButtonElement::~TabCloseButtonElement()
{
}

void TabCloseButtonElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    layout();
}

void TabCloseButtonElement::layout()
{
    m_elementList = prepareElements(&m_styleOption, m_widget, {ElementString::Tab, ElementString::CloseButton});
    if (!m_elementList.isEmpty()) {
        m_elementProperties = queryProperties(m_elementList);
        m_layoutMap[ElementString::Icon].rect = m_styleOption.rect;
        setIcon(m_style->unionIcon(m_elementProperties, (u"window-close-symbolic"_s)));
        m_isValid = true;
    } else {
        m_isValid = false;
    }
}

void TabCloseButtonElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.PrimitiveElement) {
    case QStyle::PE_IndicatorTabClose:
        drawBackground(painter);
        drawIcon(painter);
        break;
    }
}

qreal TabCloseButtonElement::pixelMetric(QStyle::PixelMetric metric) const
{
    switch (metric) {
    case QStyle::PM_TabCloseIndicatorWidth:
        return width();
    case QStyle::PM_TabCloseIndicatorHeight:
        return height();
    default:
        break;
    }
    return 0;
}
