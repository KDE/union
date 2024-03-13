/**
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>
#include <optional>

#include "union_export.h"

namespace Union
{

/**
 * @todo write docs
 */
class UNION_EXPORT Size
{
public:
    enum class Unit {
        Unknown,
        LogicalPixels,
        DevicePixels,
        Millimeter,
    };

    Size();
    Size(double value, Unit unit);

    double value() const;
    void setValue(double newValue);

    double valueAs(Unit unit);

    Unit unit() const;
    void setUnit(Unit newUnit);

private:
    double m_value = 0.0;
    Unit m_unit = Unit::Unknown;
};

}
