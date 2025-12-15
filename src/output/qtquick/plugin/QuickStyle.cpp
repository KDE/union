// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "QuickStyle.h"

#include <QCoreApplication>
#include <QQmlEngine>

#include <Element.h>
#include <EventHelper.h>
#include <StyleRule.h>

#include "QuickElement.h"

using namespace Union;

static EventTypeRegistration<QuickStyleUpdatedEvent> quickStyleRegistration;

QuickStyle::QuickStyle(QQmlEngine *engine, QObject *parent)
    : QQuickAttachedPropertyPropagator(parent)
    , m_engine(engine)
{
    m_properties = std::make_unique<StylePropertyGroup>(this);

    initialize();

    update();
}

StylePropertyGroup *QuickStyle::properties() const
{
    return m_properties.get();
}

Union::ElementQuery *QuickStyle::query() const
{
    if (m_element) {
        return m_element->query();
    }

    return nullptr;
}

QQmlEngine *QuickStyle::engine() const
{
    return m_engine;
}

QuickStyle *QuickStyle::qmlAttachedProperties(QObject *parent)
{
    return new QuickStyle(qmlEngine(parent), parent);
}

void QuickStyle::attachedParentChange(QQuickAttachedPropertyPropagator *, QQuickAttachedPropertyPropagator *)
{
    update();
}

bool QuickStyle::event(QEvent *event)
{
    if (event->type() == QuickStyleUpdatedEvent::s_type) {
        // Propagate to parent to also enable event filters on the parent to
        // notify of changes.
        QCoreApplication::sendEvent(parent(), event);
    }

    return QObject::event(event);
}

bool QuickStyle::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_element && event->type() == QuickElementUpdatedEvent::s_type) {
        update();
        // Allow the event to be processed by other event filters.
        return false;
    }

    return QObject::eventFilter(watched, event);
}

void QuickStyle::classBegin()
{
}

void QuickStyle::componentComplete()
{
    m_completed = true;
    update();
}

void QuickStyle::setElement(QuickElement *newElement)
{
    if (newElement == m_element) {
        return;
    }

    if (m_element) {
        m_element->removeEventFilter(this);
    }

    m_element = newElement;

    if (m_element) {
        m_element->installEventFilter(this);
    }
}

void QuickStyle::update()
{
    auto element = qobject_cast<QuickElement *>(qmlAttachedPropertiesObject<QuickElement>(parent(), false));
    auto parentStyle = attachedParent();

    while (!element && parentStyle) {
        element = qobject_cast<QuickElement *>(qmlAttachedPropertiesObject<QuickElement>(parentStyle->parent(), false));
        parentStyle = parentStyle->attachedParent();
    }

    setElement(element);
    if (!element) {
        return;
    }

    auto query = element->query();
    if (!query || !query->hasMatches()) {
        return;
    }

    m_properties->update(query->properties());

    QuickStyleUpdatedEvent event;
    // Send to self to allow event filtering on this instance to react to changes.
    QCoreApplication::sendEvent(this, &event);

    Q_EMIT updated();

    const auto children = attachedChildren();
    for (auto child : children) {
        qobject_cast<QuickStyle *>(child)->update();
    }
}

QuickStyleUpdatedEvent::QuickStyleUpdatedEvent()
    : QEvent(s_type)
{
}
