// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "IndicatorSubElement.h"

#include <QApplication>

using namespace Union::Widgets;
using namespace Union::Properties;

IndicatorSubElement::IndicatorSubElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
    setFlag(AbstractElementFlag::SubElement);
}

QIcon IndicatorSubElement::icon() const
{
    return m_icon;
}

void IndicatorSubElement::setIcon(const QIcon &icon)
{
    if (icon.cacheKey() == m_icon.cacheKey()) {
        return;
    }

    m_icon = icon;
    markDirty();
}

// QSize IndicatorSubElement::size() const
// {
//     return m_size;
// }
//
// void IndicatorSubElement::setSize(QSize size)
// {
//     if (size == m_size) {
//         return;
//     }
//
//     m_size = size;
//     markDirty();
// }

void IndicatorSubElement::update(const QStyleOption *option)
{
    auto e = element();
    if (!e) {
        e = Union::Element::create();
        e->setType(ElementString::Indicator);
        setElement(e);
    }

    AbstractElement::update(option);
}

void IndicatorSubElement::draw(QPainter *painter, [[maybe_unused]] DrawVariant what) const
{
    drawBackground(painter);

    if (m_icon.isNull()) {
        return;
    }

    auto unionIcon = style()->unionIcon(properties(), m_icon.name());
    // Fallback if union icon cant be loaded
    if (unionIcon.isNull()) {
        unionIcon = m_icon;
    }

    const qreal dpr = painter->device() ? painter->device()->devicePixelRatioF() : qApp->devicePixelRatio();
    auto iconSize = rect().size();
    const QPixmap pixmap = unionIcon.pixmap(iconSize.toSize(), dpr, m_enabled ? QIcon::Normal : QIcon::Disabled);
    painter->save();
    style()->drawItemPixmap(painter, rect(), Qt::AlignCenter, pixmap);
    painter->restore();
}
