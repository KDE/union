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

BordersGroup::BordersGroup()
    : QObject()
{
}

Sizes BordersGroup::sizes() const
{
    return m_sizes;
}

QBindable<Sizes> BordersGroup::bindableSizes()
{
    return QBindable<Sizes>(&m_sizes);
}

void BordersGroup::update(const std::optional<Union::BorderDefinition> &borders)
{
    if (!borders.has_value()) {
        m_sizes = Sizes();
        return;
    }

    auto value = borders.value();
    Sizes newSizes;
    newSizes.setLeft(value.left.value_or(LineDefinition()).size);
    newSizes.setRight(value.right.value_or(LineDefinition()).size);
    newSizes.setTop(value.top.value_or(LineDefinition()).size);
    newSizes.setBottom(value.bottom.value_or(LineDefinition()).size);
    m_sizes = newSizes;
}

StatesGroup::StatesGroup()
{
    m_activeStates.setBinding([this]() {
        Union::Element::States states;
        if (m_hovered.value()) {
            states |= Union::Element::State::Hovered;
        }
        if (m_focus.value()) {
            states |= Union::Element::State::Focus;
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

bool StatesGroup::focus() const
{
    return m_focus;
}

void StatesGroup::setFocus(bool newFocus)
{
    m_focus = newFocus;
}

QBindable<bool> StatesGroup::bindableFocus()
{
    return QBindable<bool>(&m_focus);
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

    m_bordersGroup = std::make_unique<BordersGroup>();
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

Sizes QuickElement::padding() const
{
    return m_padding;
}

QBindable<Sizes> QuickElement::bindablePadding()
{
    return QBindable<Sizes>(&m_padding);
}

Sizes QuickElement::margins() const
{
    return m_margins;
}

QBindable<Sizes> QuickElement::bindableMargins()
{
    return QBindable<Sizes>(&m_margins);
}

StatesGroup *QuickElement::states() const
{
    return m_statesGroup.get();
}

BordersGroup *QuickElement::borders() const
{
    return m_bordersGroup.get();
}

qreal QuickElement::implicitWidth() const
{
    return m_implicitWidth;
}

QBindable<qreal> QuickElement::bindableImplicitWidth()
{
    return QBindable<qreal>(&m_implicitWidth);
}

qreal QuickElement::implicitHeight() const
{
    return m_implicitHeight;
}

QBindable<qreal> QuickElement::bindableImplicitHeight()
{
    return QBindable<qreal>(&m_implicitHeight);
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
    update();
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

    auto rect = m_query.boundingRect();

    m_implicitWidth = rect.width();
    m_implicitHeight = rect.height();

    m_margins = Sizes(m_query.margins());
    m_padding = Sizes(m_query.padding());

    m_bordersGroup->update(m_query.border());

    Q_EMIT updated();

    const auto children = attachedChildren();
    for (auto child : children) {
        qobject_cast<QuickElement *>(child)->update();
    }
}
