// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "QuickElement.h"

#include <QQmlEngine>

#include "Element.h"
#include "StyleRule.h"
#include "Theme.h"
#include "ThemeRegistry.h"

#include "QuickStyle.h"

using namespace Union;

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
    Q_EMIT highlightedChanged();
    Q_EMIT activeStatesChanged();
    m_parent->setActiveStates(m_activeStates);
}

QuickElement::QuickElement(QObject *parent)
    : QQuickAttachedPropertyPropagator(parent)
{
    m_element = Element::create();
    connect(m_element.get(), &Element::typeChanged, this, &QuickElement::typeChanged);
    connect(m_element.get(), &Element::idChanged, this, &QuickElement::elementIdChanged);
    connect(m_element.get(), &Element::colorSetChanged, this, &QuickElement::colorSetChanged);
    connect(m_element.get(), &Element::hintsChanged, this, &QuickElement::hintsChanged);
    connect(m_element.get(), &Element::attributesChanged, this, &QuickElement::attributesChanged);
    connect(m_element.get(), &Element::updated, this, &QuickElement::update);

    m_statesGroup = std::make_unique<StatesGroup>(this);

    initialize();
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
    // The attached parent may not yet have been fully initialized when this is
    // called, so delay the update momentarily so the parent can be updated
    // first.
    QMetaObject::invokeMethod(this, &QuickElement::update, Qt::QueuedConnection);
}

void QuickElement::setActiveStates(Union::Element::States newActiveStates)
{
    m_element->setStates(newActiveStates);
}

void QuickElement::update()
{
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

    Q_EMIT updated();

    const auto children = attachedChildren();
    for (auto child : children) {
        qobject_cast<QuickElement *>(child)->update();
    }
}
