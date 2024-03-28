/**
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Size.h"

#include <QString>

using namespace Union;
using namespace Qt::StringLiterals;

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

QDebug operator<<(QDebug debug, const Union::Size &size)
{
    QDebugStateSaver saver(debug);

    auto unitString = u"(Unknown Unit)"_qs;
    switch (size.unit()) {
    case Size::Unit::LogicalPixels:
        unitString = u"lp"_qs;
        break;
    case Size::Unit::DevicePixels:
        unitString = u"dp"_qs;
        break;
    case Size::Unit::Millimeter:
        unitString = u"mm"_qs;
        break;
    case Size::Unit::Unknown:
        break;
    }

    debug.nospace() << "Size(" << size.value() << " " << qPrintable(unitString) << ")";
    return debug;
}
