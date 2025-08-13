// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "QuickElement.h"

#include <QCoreApplication>
#include <QQmlEngine>

#include "Element.h"
#include "EventHelper.h"
#include "StyleRule.h"
#include "Theme.h"
#include "ThemeRegistry.h"

#include "QuickStyle.h"

using namespace Union;

static EventTypeRegistration<QuickElementUpdatedEvent> quickElementRegistration;

StatesGroup::StatesGroup(QuickElement *parent)
    : m_parent(parent)
{
}

bool StatesGroup::hovered() const
{
    return m_activeStates.testFlag(Element::State::Hovered);
}

void StatesGroup::setHovered(bool newHovered)
{
    setState(Element::State::Hovered, newHovered);
}

bool StatesGroup::activeFocus() const
{
    return m_activeStates.testFlag(Element::State::ActiveFocus);
}

void StatesGroup::setActiveFocus(bool newActiveFocus)
{
    setState(Element::State::ActiveFocus, newActiveFocus);
}

bool StatesGroup::visualFocus() const
{
    return m_activeStates.testFlag(Element::State::VisualFocus);
}

void StatesGroup::setVisualFocus(bool newVisualFocus)
{
    setState(Element::State::VisualFocus, newVisualFocus);
}

bool StatesGroup::pressed() const
{
    return m_activeStates.testFlag(Element::State::Pressed);
}

void StatesGroup::setPressed(bool newPressed)
{
    setState(Element::State::Pressed, newPressed);
}

bool StatesGroup::checked() const
{
    return m_activeStates.testFlag(Element::State::Checked);
}

void StatesGroup::setChecked(bool newChecked)
{
    setState(Element::State::Checked, newChecked);
}

bool StatesGroup::enabled() const
{
    return !m_activeStates.testFlag(Element::State::Disabled);
}

void StatesGroup::setEnabled(bool newEnabled)
{
    setState(Element::State::Disabled, !newEnabled);
}

bool StatesGroup::highlighted() const
{
    return m_activeStates & Element::State::Highlighted;
}

void StatesGroup::setHighlighted(bool newHighlighted)
{
    setState(Element::State::Highlighted, newHighlighted);
}

Union::Element::States StatesGroup::activeStates() const
{
    return m_activeStates;
}

void StatesGroup::setState(Union::Element::State state, bool set)
{
    auto newStates = m_activeStates;
    newStates.setFlag(state, set);

    if (newStates == m_activeStates) {
        return;
    }

    m_activeStates = newStates;

    emitStateChange(state);
    Q_EMIT activeStatesChanged();
    m_parent->setActiveStates(m_activeStates);
}

void StatesGroup::setActiveStates(Element::States states)
{
    if (states == m_activeStates) {
        return;
    }

    m_activeStates = states;
    emitStateChange(m_activeStates);
    Q_EMIT activeStatesChanged();
}

void StatesGroup::emitStateChange(Union::Element::States states)
{
    if (states & Element::State::Hovered) {
        Q_EMIT hoveredChanged();
    }
    if (states & Element::State::ActiveFocus) {
        Q_EMIT activeFocusChanged();
    }
    if (states & Element::State::VisualFocus) {
        Q_EMIT visualFocusChanged();
    }
    if (states & Element::State::Pressed) {
        Q_EMIT pressedChanged();
    }
    if (states & Element::State::Checked) {
        Q_EMIT checkedChanged();
    }
    if (states & Element::State::Highlighted) {
        Q_EMIT highlightedChanged();
    }
    if (states & Element::State::Disabled) {
        Q_EMIT enabledChanged();
    }
}

QuickElement::QuickElement(QObject *parent)
    : QQuickAttachedPropertyPropagator(parent)
{
    m_element = Element::create();
    m_element->installEventFilter(this);

    m_statesGroup = std::make_unique<StatesGroup>(this);

    initialize();

    update();
}

QString QuickElement::type() const
{
    return m_element->type();
}

void QuickElement::setType(const QString &newType)
{
    m_element->setType(newType);
}

QString QuickElement::elementId() const
{
    return m_element->id();
}

void QuickElement::setElementId(const QString &newId)
{
    m_element->setId(newId);
}

Union::Element::ColorSet QuickElement::colorSet() const
{
    return m_element->colorSet();
}

void QuickElement::setColorSet(Union::Element::ColorSet newColorSet)
{
    m_element->setColorSet(Element::ColorSet(newColorSet));
}

QStringList QuickElement::hints() const
{
    const auto h = m_element->hints();
    return QStringList(h.begin(), h.end());
}

void QuickElement::setHints(const QStringList &newHints)
{
    m_element->setHints(QSet<QString>(newHints.begin(), newHints.end()));
}

QVariantMap QuickElement::attributes() const
{
    return m_element->attributes();
}

void QuickElement::setAttributes(const QVariantMap &newAttributes)
{
    m_element->setAttributes(newAttributes);
}

StatesGroup *QuickElement::states() const
{
    return m_statesGroup.get();
}

Union::ElementQuery *QuickElement::query() const
{
    return m_query.get();
}

QuickElement *QuickElement::qmlAttachedProperties(QObject *parent)
{
    return new QuickElement(parent);
}

void QuickElement::attachedParentChange(QQuickAttachedPropertyPropagator *, QQuickAttachedPropertyPropagator *)
{
    update();
}

bool QuickElement::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_element.get() && event->type() == ElementChangedEvent::s_type) {
        auto changeEvent = static_cast<ElementChangedEvent *>(event);
        if (changeEvent->changes & Element::Change::Type) {
            Q_EMIT typeChanged();
        }
        if (changeEvent->changes & Element::Change::Id) {
            Q_EMIT elementIdChanged();
        }
        if (changeEvent->changes & Element::Change::States) {
            m_statesGroup->setActiveStates(m_element->states());
        }
        if (changeEvent->changes & Element::Change::Hints) {
            Q_EMIT hintsChanged();
        }
        if (changeEvent->changes & Element::Change::Attributes) {
            Q_EMIT attributesChanged();
        }

        update();
    }

    return QObject::eventFilter(watched, event);
}

void QuickElement::classBegin()
{
}

void QuickElement::componentComplete()
{
    m_completed = true;
    update();
}

void QuickElement::setActiveStates(Union::Element::States newActiveStates)
{
    m_element->setStates(newActiveStates);
}

void QuickElement::update()
{
    if (!m_completed) {
        return;
    }

    auto theme = ThemeRegistry::instance()->defaultTheme();

    m_query = std::make_unique<Union::ElementQuery>(theme);

    QList<Element::Ptr> elements;
    elements.append(m_element);

    auto attached = qobject_cast<QuickElement *>(attachedParent());
    while (attached) {
        elements.prepend(attached->m_element);
        attached = qobject_cast<QuickElement *>(attached->attachedParent());
    }

    m_query->setElements(elements);
    m_query->execute();

    if (m_query->hasMatches()) {
        // Force creation of a style attached object to ensure style properties
        // are read from this element rather than something further up the
        // hierarchy.
        qmlAttachedPropertiesObject<QuickStyle>(parent(), true);
    }

    QuickElementUpdatedEvent event;
    QCoreApplication::sendEvent(this, &event);

    Q_EMIT updated();

    const auto children = attachedChildren();
    for (auto child : children) {
        qobject_cast<QuickElement *>(child)->update();
    }
}

QuickElementUpdatedEvent::QuickElementUpdatedEvent()
    : QEvent(QuickElementUpdatedEvent::s_type)
{
}
