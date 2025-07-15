// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "Units.h"

#include "ElementQuery.h"

using namespace Qt::StringLiterals;

Units::Units(QObject *parent)
    : Kirigami::Platform::Units(parent)
{
    // TODO: Find a way to get something like this to work:
    //
    // auto unitsElement = Union::Element::create();
    // unitsElement->setType(u"KirigamiUnits"_s);
    //
    // Union::ElementQuery query;
    // query.setElements({unitsElement});
    // query.execute();

    setVeryShortDuration(50);
    setShortDuration(100);
    setLongDuration(200);
    setVeryLongDuration(400);
}
