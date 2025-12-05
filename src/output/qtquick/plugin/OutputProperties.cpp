// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

#include "OutputProperties.h"

#include <QCoreApplication>
#include <QQmlEngine>

#include <Element.h>
#include <EventHelper.h>
#include <StyleRule.h>
#include <Theme.h>

using namespace Union;

static EventTypeRegistration<OutputPropertiesUpdatedEvent> quickStyleRegistration;

OutputProperties::OutputProperties(QQmlEngine *engine, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
{
    update();
}

bool OutputProperties::useAlternatingColors() const
{
    return m_useAlternatingColors;
}

void OutputProperties::setUseAlternatingColors(bool newUseAlternatingColors)
{
    if (m_useAlternatingColors != newUseAlternatingColors) {
        m_useAlternatingColors = newUseAlternatingColors;
        update();
        Q_EMIT useAlternatingColorsChanged();
    }
}

OutputProperties *OutputProperties::qmlAttachedProperties(QObject *parent)
{
    return new OutputProperties(qmlEngine(parent), parent);
}

void OutputProperties::update()
{
    OutputPropertiesUpdatedEvent event;
    // Send to self to allow event filtering on this instance to react to changes.
    QCoreApplication::sendEvent(this, &event);

    Q_EMIT updated();
}

OutputPropertiesUpdatedEvent::OutputPropertiesUpdatedEvent()
    : QEvent(s_type)
{
}
