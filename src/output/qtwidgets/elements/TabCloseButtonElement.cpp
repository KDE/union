// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "TabCloseButtonElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

TabCloseButtonElement::TabCloseButtonElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
}

void TabCloseButtonElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    layout();
}

void TabCloseButtonElement::layout()
{
    m_elementList = prepareElements(option, widget(), {ElementString::Tab, ElementString::CloseButton});
    if (!m_elementList.isEmpty()) {
        m_elementProperties = queryProperties(m_elementList);
        if (!option) {
            QStyleOption opt;
            if (widget()) {
                opt.initFrom(widget());
            }
            m_layoutMap[ElementString::Icon].rect = opt.rect;
        } else {
            m_layoutMap[ElementString::Icon].rect = option->rect;
        }
        setIcon(style()->unionIcon(m_elementProperties, (u"window-close-symbolic"_s)));
        m_isValid = true;
    } else {
        m_isValid = false;
    }
}

void TabCloseButtonElement::draw(QPainter *painter, DrawVariant what) const
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

qreal TabCloseButtonElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric metric) const
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
