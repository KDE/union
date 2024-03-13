/**
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Size.h"

using namespace Union;

Size::Size()
{
}

Size::Size(double value, Size::Unit unit)
    : m_value(value)
    , m_unit(unit)
{
}

double Size::value() const
{
    return m_value;
}

void Size::setValue(double newValue)
{
    m_value = newValue;
}

double Size::valueAs(Size::Unit unit)
{
    // TODO: Implement
    return -1;
}

Size::Unit Size::unit() const
{
    return m_unit;
}

void Size::setUnit(Size::Unit newUnit)
{
    m_unit = newUnit;
}
