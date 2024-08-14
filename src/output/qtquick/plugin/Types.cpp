/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Types.h"

Sizes::Sizes()
    : Sizes(0.0, 0.0, 0.0, 0.0)
{
}

Sizes::Sizes(qreal left, qreal right, qreal top, qreal bottom)
    : m_margins(left, top, right, bottom)
{
}

Sizes::Sizes(const QMarginsF &margins)
    : m_margins(margins)
{
}

bool Sizes::operator==(const Sizes &other)
{
    return m_margins == other.m_margins;
}

qreal Sizes::left() const
{
    return m_margins.left();
}

void Sizes::setLeft(qreal newLeft)
{
    m_margins.setLeft(newLeft);
}

qreal Sizes::right() const
{
    return m_margins.right();
}

void Sizes::setRight(qreal newRight)
{
    m_margins.setRight(newRight);
}

qreal Sizes::top() const
{
    return m_margins.top();
}

void Sizes::setTop(qreal newTop)
{
    m_margins.setTop(newTop);
}

qreal Sizes::bottom() const
{
    return m_margins.bottom();
}

void Sizes::setBottom(qreal newBottom)
{
    m_margins.setBottom(newBottom);
}

bool Sizes::isValid() const
{
    return !m_margins.isNull();
}

QMarginsF Sizes::toMargins() const
{
    return m_margins;
}
