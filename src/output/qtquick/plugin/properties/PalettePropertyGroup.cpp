// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `generate_properties.py`.

#include "PalettePropertyGroup.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

PalettePropertyGroup::PalettePropertyGroup()
{
}

void PalettePropertyGroup::update(const PaletteProperty &newState)
{
    m_foreground = newState.foreground().value_or(QColor{});
    m_background = newState.background().value_or(QColor{});
    m_decoration = newState.decoration().value_or(QColor{});
}

QColor PalettePropertyGroup::foreground() const
{
    return m_foreground;
}

void PalettePropertyGroup::setForeground(const QColor &newValue)
{
    m_foreground = newValue;
}

QBindable<QColor> PalettePropertyGroup::bindableForeground()
{
    return QBindable<QColor>(&m_foreground);
}

QColor PalettePropertyGroup::background() const
{
    return m_background;
}

void PalettePropertyGroup::setBackground(const QColor &newValue)
{
    m_background = newValue;
}

QBindable<QColor> PalettePropertyGroup::bindableBackground()
{
    return QBindable<QColor>(&m_background);
}

QColor PalettePropertyGroup::decoration() const
{
    return m_decoration;
}

void PalettePropertyGroup::setDecoration(const QColor &newValue)
{
    m_decoration = newValue;
}

QBindable<QColor> PalettePropertyGroup::bindableDecoration()
{
    return QBindable<QColor>(&m_decoration);
}

#include "moc_PalettePropertyGroup.cpp"