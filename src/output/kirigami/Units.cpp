// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "Units.h"

#include "ElementQuery.h"
#include "StyleRegistry.h"

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

    auto platform = Union::StyleRegistry::instance()->platform();
    connect(platform.get(), &Union::PlatformPlugin::animationSpeedMultiplierChanged, this, &Units::setAnimationSpeeds);
    setAnimationSpeeds();
}

void Units::setAnimationSpeeds()
{
    auto multiplier = Union::StyleRegistry::instance()->platform()->animationSpeedMultiplier();
    setVeryShortDuration(50 * multiplier);
    setShortDuration(100 * multiplier);
    setLongDuration(200 * multiplier);
    setVeryLongDuration(400 * multiplier);
}

#include "moc_Units.cpp"
