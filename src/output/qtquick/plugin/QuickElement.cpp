/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "QuickElement.h"

#include "Element.h"
#include "Style.h"
#include "Theme.h"

using namespace Union;

StatesGroup::StatesGroup()
{
    m_activeStates.setBinding([this]() {
        Union::Element::States states;
        if (m_hovered.value()) {
            states |= Union::Element::State::Hovered;
        }
        if (m_activeFocus.value()) {
            states |= Union::Element::State::ActiveFocus;
        }
        if (m_visualFocus.value()) {
            states |= Union::Element::State::VisualFocus;
        }
        if (m_pressed.value()) {
            states |= Union::Element::State::Pressed;
        }
        if (m_checked.value()) {
            states |= Union::Element::State::Checked;
        }
        if (!m_enabled.value()) {
            states |= Union::Element::State::Disabled;
        }
        if (m_highlighted.value()) {
            states |= Union::Element::State::Highlighted;
        }
        return states;
    });
}

bool StatesGroup::hovered() const
{
    return m_hovered;
}

void StatesGroup::setHovered(bool newHovered)
{
    m_hovered = newHovered;
}

QBindable<bool> StatesGroup::bindableHovered()
{
    return QBindable<bool>(&m_hovered);
}

bool StatesGroup::activeFocus() const
{
    return m_activeFocus;
}

void StatesGroup::setActiveFocus(bool newActiveFocus)
{
    m_activeFocus = newActiveFocus;
}

QBindable<bool> StatesGroup::bindableActiveFocus()
{
    return QBindable<bool>(&m_activeFocus);
}

bool StatesGroup::visualFocus() const
{
    return m_visualFocus;
}

void StatesGroup::setVisualFocus(bool newVisualFocus)
{
    m_visualFocus = newVisualFocus;
}

QBindable<bool> StatesGroup::bindableVisualFocus()
{
    return QBindable<bool>(&m_visualFocus);
}

bool StatesGroup::pressed() const
{
    return m_pressed;
}

void StatesGroup::setPressed(bool newPressed)
{
    m_pressed = newPressed;
}

QBindable<bool> StatesGroup::bindablePressed()
{
    return QBindable<bool>(&m_pressed);
}

bool StatesGroup::checked() const
{
    return m_checked;
}

void StatesGroup::setChecked(bool newChecked)
{
    m_checked = newChecked;
}

QBindable<bool> StatesGroup::bindableChecked()
{
    return QBindable<bool>(&m_checked);
}

bool StatesGroup::enabled() const
{
    return m_enabled;
}

void StatesGroup::setEnabled(bool newEnabled)
{
    m_enabled = newEnabled;
}

QBindable<bool> StatesGroup::bindableEnabled()
{
    return QBindable<bool>(&m_enabled);
}

bool StatesGroup::highlighted() const
{
    return m_highlighted;
}

void StatesGroup::setHighlighted(bool newHighlighted)
{
    m_highlighted = newHighlighted;
}

QBindable<bool> StatesGroup::bindableHighlighted()
{
    return QBindable<bool>(&m_highlighted);
}

Union::Element::States StatesGroup::activeStates() const
{
    return m_activeStates;
}

QBindable<Union::Element::States> StatesGroup::bindableActiveStates()
{
    return QBindable<Union::Element::States>(&m_activeStates);
}

QuickElement::QuickElement(QObject *parent)
    : QQuickAttachedPropertyPropagator(parent)
{
    m_element = Element::create();

    m_statesGroup = std::make_unique<StatesGroup>();

    m_element->bindableStates().setBinding([this]() {
        return m_statesGroup->bindableActiveStates().value();
    });

    m_activeStatesNotifier = m_statesGroup->bindableActiveStates().addNotifier([this]() {
        update();
    });

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

QBindable<QString> QuickElement::bindableType()
{
    return m_element->bindableType();
}

QString QuickElement::elementId() const
{
    return m_element->id();
}

void QuickElement::setElementId(const QString &newId)
{
    m_element->setId(newId);
}

QBindable<QString> QuickElement::bindableElementId()
{
    return m_element->bindableId();
}

Union::Element::ColorSet QuickElement::colorSet() const
{
    return m_element->colorSet();
}

void QuickElement::setColorSet(Union::Element::ColorSet newColorSet)
{
    m_element->setColorSet(Element::ColorSet(newColorSet));
}

QBindable<Union::Element::ColorSet> QuickElement::bindableColorSet()
{
    return m_element->bindableColorSet();
}

QStringList QuickElement::hints() const
{
    return m_element->hints();
}

void QuickElement::setHints(const QStringList &newHints)
{
    m_element->setHints(newHints);
}

QBindable<QStringList> QuickElement::bindableHints()
{
    return m_element->bindableHints();
}

QVariantMap QuickElement::attributes() const
{
    return m_element->attributes();
}

void QuickElement::setAttributes(const QVariantMap &newAttributes)
{
    m_element->setAttributes(newAttributes);
}

QBindable<QVariantMap> QuickElement::bindableAttributes()
{
    return m_element->bindableAttributes();
}

StatesGroup *QuickElement::states() const
{
    return m_statesGroup.get();
}

Union::ElementQuery QuickElement::query() const
{
    return m_query;
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

void QuickElement::update()
{
    m_query = Union::ElementQuery(Union::Theme::instance());

    QList<Element::Ptr> elements;
    elements.append(m_element);

    auto parent = qobject_cast<QuickElement *>(attachedParent());
    while (parent) {
        elements.prepend(parent->m_element);
        parent = qobject_cast<QuickElement *>(parent->attachedParent());
    }

    m_query.setElements(elements);
    m_query.execute();

    Q_EMIT updated();

    const auto children = attachedChildren();
    for (auto child : children) {
        qobject_cast<QuickElement *>(child)->update();
    }
}
