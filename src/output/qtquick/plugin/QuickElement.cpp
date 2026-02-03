// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "QuickElement.h"

#include <QCoreApplication>
#include <QQmlEngine>

#include "Element.h"
#include "EventHelper.h"
#include "Style.h"
#include "StyleRegistry.h"
#include "StyleRule.h"

#include "QuickStyle.h"

using namespace Union;

static EventTypeRegistration<QuickElementUpdatedEvent> quickElementRegistration;

template<typename T, QList<T *> QuickElement::*member, void (QuickElement::*changeSignal)()>
struct ListFunctions {
    static void append(QQmlListProperty<T> *property, T *value)
    {
        if (!value) {
            return;
        }

        auto element = static_cast<QuickElement *>(property->object);
        auto list = &(element->*member);
        list->append(value);
        value->setElement(element);
        (element->*changeSignal)();
    }

    static qsizetype count(QQmlListProperty<T> *property)
    {
        auto list = static_cast<QuickElement *>(property->object)->*member;
        return list.count();
    }

    static T *at(QQmlListProperty<T> *property, qsizetype index)
    {
        auto list = static_cast<QuickElement *>(property->object)->*member;
        return list.at(index);
    }

    static void clear(QQmlListProperty<T> *property)
    {
        auto element = static_cast<QuickElement *>(property->object);
        auto list = &(element->*member);
        list->clear();
        (element->*changeSignal)();
    }

    static void replace(QQmlListProperty<T> *property, qsizetype index, T *value)
    {
        if (!value) {
            return;
        }

        auto element = static_cast<QuickElement *>(property->object);
        auto list = &(element->*member);
        list->replace(index, value);
        (element->*changeSignal)();
    }

    static void removeLast(QQmlListProperty<T> *property)
    {
        auto element = static_cast<QuickElement *>(property->object);
        auto list = &(element->*member);
        list->removeLast();
        (element->*changeSignal)();
    }
};

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

ElementHint::ElementHint(QObject *parent)
    : QObject(parent)
{
}

QString ElementHint::name() const
{
    return m_name;
}

void ElementHint::setName(const QString &newName)
{
    if (newName == m_name) {
        return;
    }

    m_name = newName;
    update();
    Q_EMIT nameChanged();
}

bool ElementHint::when() const
{
    return m_when;
}

void ElementHint::setWhen(bool newWhen)
{
    if (newWhen == m_when) {
        return;
    }

    m_when = newWhen;
    update();
    Q_EMIT whenChanged();
}

void ElementHint::setElement(QuickElement *element)
{
    m_element = element;
    update();
}

void ElementHint::update()
{
    if (m_element) {
        m_element->updateHints();
    }
}

ElementAttribute::ElementAttribute(QObject *parent)
    : ElementHint(parent)
{
}

QVariant ElementAttribute::value() const
{
    return m_value;
}

void ElementAttribute::setValue(const QVariant &newValue)
{
    if (newValue == m_value) {
        return;
    }

    m_value = newValue;
    update();
    Q_EMIT valueChanged();
}

void ElementAttribute::update()
{
    if (m_element) {
        m_element->updateAttributes();
    }
}

void ElementAttribute::resetValue()
{
    setValue(QVariant{});
}

QuickElement::QuickElement(QObject *parent)
    : QQuickAttachedPropertyPropagator(parent)
{
    m_element = Element::create();
    m_element->installEventFilter(this);

    m_statesGroup = std::make_unique<StatesGroup>(this);
    m_style = StyleRegistry::instance()->defaultStyle();

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
    m_element->setHints(newHints);
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

QuickElement *QuickElement::parentElement() const
{
    return static_cast<QuickElement *>(attachedParent());
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
        return false;
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

std::shared_ptr<Union::Style> QuickElement::style() const
{
    return m_style;
}

void QuickElement::update()
{
    if (!m_completed) {
        return;
    }

    m_query = std::make_unique<Union::ElementQuery>(m_style);

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
