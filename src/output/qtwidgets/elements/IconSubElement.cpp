// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "IconSubElement.h"

#include <QApplication>

using namespace Union::Widgets;
using namespace Union::Properties;

IconSubElement::IconSubElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
    setFlag(AbstractElementFlag::SubElement);
}

QIcon IconSubElement::icon() const
{
    return m_icon;
}

void IconSubElement::setIcon(const QIcon &icon)
{
    if (icon.cacheKey() == m_icon.cacheKey()) {
        return;
    }

    m_icon = icon;
    markDirty();
}

QSize IconSubElement::size() const
{
    return m_size;
}

void IconSubElement::setSize(QSize size)
{
    if (size == m_size) {
        return;
    }

    m_size = size;
    markDirty();
}

void IconSubElement::update(const QStyleOption *option)
{
    m_color = option->palette.text().color();
    if (option->palette.resolveMask() <= 0) {
        auto unionColor = safePropertyLookup(properties(), Union::Color{}, &StylePropertyGroup::text, &TextPropertyGroup::color);
        if (unionColor.isValid()) {
            m_color = unionColor.toQColor();
        }
    }

    m_enabled = option->state.testFlag(QStyle::State_Enabled);

    AbstractElement::update(option);
}

void IconSubElement::draw(QPainter *painter, [[maybe_unused]] DrawVariant what) const
{
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
