// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "ExtraData.h"

#include <QCoreApplication>
#include <QQmlEngine>

#include <Element.h>
#include <EventHelper.h>
#include <StyleRule.h>
#include <Theme.h>

using namespace Union;

static EventTypeRegistration<ExtraDataUpdatedEvent> quickStyleRegistration;

ExtraData::ExtraData(QQmlEngine *engine, QObject *parent)
    : QQuickAttachedPropertyPropagator(parent)
    , m_engine(engine)
{
    initialize();

    update();
}

QVariantMap ExtraData::data() const
{
    return m_extraData;
}

void ExtraData::setData(const QVariantMap &newData)
{
    m_extraData.insert(newData);
}

QQmlEngine *ExtraData::engine() const
{
    return m_engine;
}

ExtraData *ExtraData::qmlAttachedProperties(QObject *parent)
{
    return new ExtraData(qmlEngine(parent), parent);
}

void ExtraData::attachedParentChange(QQuickAttachedPropertyPropagator *, QQuickAttachedPropertyPropagator *)
{
    update();
}

bool ExtraData::event(QEvent *event)
{
    if (event->type() == ExtraDataUpdatedEvent::s_type) {
        // Propagate to parent to also enable event filters on the parent to
        // notify of changes.
        QCoreApplication::sendEvent(parent(), event);
    }

    return QObject::event(event);
}

bool ExtraData::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == ExtraDataUpdatedEvent::s_type) {
        update();
        // Allow the event to be processed by other event filters.
        return false;
    }

    return QObject::eventFilter(watched, event);
}

void ExtraData::classBegin()
{
}

void ExtraData::componentComplete()
{
    m_completed = true;
    update();
}

void ExtraData::update()
{
    ExtraDataUpdatedEvent event;
    // Send to self to allow event filtering on this instance to react to changes.
    QCoreApplication::sendEvent(this, &event);

    Q_EMIT updated();

    const auto children = attachedChildren();
    for (auto child : children) {
        qobject_cast<ExtraData *>(child)->update();
    }
}

ExtraDataUpdatedEvent::ExtraDataUpdatedEvent()
    : QEvent(s_type)
{
}
