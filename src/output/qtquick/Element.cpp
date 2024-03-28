/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Element.h"

#include "ElementIdentifier.h"
#include "Style.h"

using namespace Union;

BordersGroup::BordersGroup(Element *parent)
    : QObject()
    , m_parent(parent)
{
    connect(m_parent, &Element::stateChanged, this, &BordersGroup::changed);
}

qreal BordersGroup::leftSize() const
{
    return m_parent->styleElement() ? m_parent->styleElement()->borderSizes().left() : 0.0;
}

qreal BordersGroup::rightSize() const
{
    return m_parent->styleElement() ? m_parent->styleElement()->borderSizes().right() : 0.0;
}

qreal BordersGroup::topSize() const
{
    return m_parent->styleElement() ? m_parent->styleElement()->borderSizes().top() : 0.0;
}

qreal BordersGroup::bottomSize() const
{
    return m_parent->styleElement() ? m_parent->styleElement()->borderSizes().bottom() : 0.0;
}

PaddingGroup::PaddingGroup(Element *parent)
    : QObject()
    , m_parent(parent)
{
}

qreal PaddingGroup::left() const
{
    return m_parent->styleElement() ? m_parent->styleElement()->padding().value_or(SizeDefinition{}).left.value() : 0.0;
}

qreal PaddingGroup::right() const
{
    return m_parent->styleElement() ? m_parent->styleElement()->padding().value_or(SizeDefinition{}).right.value() : 0.0;
}

qreal PaddingGroup::top() const
{
    return m_parent->styleElement() ? m_parent->styleElement()->padding().value_or(SizeDefinition{}).top.value() : 0.0;
}

qreal PaddingGroup::bottom() const
{
    return m_parent->styleElement() ? m_parent->styleElement()->padding().value_or(SizeDefinition{}).bottom.value() : 0.0;
}

Element::Element(QObject *parent)
    : QObject(parent)
{
    m_bordersGroup = std::make_unique<BordersGroup>(this);
    m_paddingGroup = std::make_unique<PaddingGroup>(this);
}

QString Element::name() const
{
    return m_name;
}

void Element::setName(const QString &newName)
{
    if (newName == m_name) {
        return;
    }

    m_name = newName;
    updateElement();
    Q_EMIT nameChanged();
}

bool Element::hovered() const
{
    return m_hovered;
}

void Element::setHovered(bool newHovered)
{
    if (newHovered == m_hovered) {
        return;
    }

    m_hovered = newHovered;
    Q_EMIT hoveredChanged();
    Q_EMIT stateChanged();
}

bool Element::focus() const
{
    return m_focus;
}

void Element::setFocus(bool newFocus)
{
    if (newFocus == m_focus) {
        return;
    }

    m_focus = newFocus;
    Q_EMIT focusChanged();
    Q_EMIT stateChanged();
}

bool Element::activeFocus() const
{
    return m_activeFocus;
}

void Element::setActiveFocus(bool newActiveFocus)
{
    if (newActiveFocus == m_activeFocus) {
        return;
    }

    m_activeFocus = newActiveFocus;
    Q_EMIT activeFocusChanged();
    Q_EMIT stateChanged();
}

bool Element::pressed() const
{
    return m_pressed;
}

void Element::setPressed(bool newPressed)
{
    if (newPressed == m_pressed) {
        return;
    }

    m_pressed = newPressed;
    Q_EMIT pressedChanged();
    Q_EMIT stateChanged();
}

PaddingGroup *Element::padding() const
{
    return m_paddingGroup.get();
}

BordersGroup *Element::borders() const
{
    return m_bordersGroup.get();
}

qreal Element::implicitWidth() const
{
    auto element = styleElement();
    return element ? element->boundingRect().width() : 0.0;
}

qreal Element::implicitHeight() const
{
    auto element = styleElement();
    return element ? element->boundingRect().height() : 0.0;
}

StyleElement::Ptr Element::styleElement() const
{
    if (m_pressed) {
        return m_pressElement;
    } else if (m_activeFocus) {
        return m_activeFocusElement;
    } else if (m_focus) {
        return m_focusElement;
    } else if (m_hovered) {
        return m_hoverElement;
    } else {
        return m_normalElement;
    }
}

// void Element::stateChanged(Element::StateChange change)
// {
//     switch (change) {
//         case Element::StateChange::ElementNameChanged:
//             break;
//         case Element::StateChange::HoveredChanged:
//             break;
//         case Element::StateChange::FocusChanged:
//             break;
//         case Element::StateChange::ActiveFocusChanged:
//             break;
//         case Element::StateChange::PressedChanged:
//             break;
//     }
// }

// #include "StyleElementQml.h"
//
// #include "Style.h"
//
// StyleElementQml::StyleElementQml(QObject* parent)
//     : StyleElement(parent)
// {
// }
//
// QString StyleElementQml::elementName() const
// {
//     return m_elementName;
// }
//
// void StyleElementQml::setElementName(const QString &newElementName)
// {
//     if (newElementName == m_elementName) {
//         return;
//     }
//
//     m_elementName = newElementName;
//     updateElement();
//     Q_EMIT elementNameChanged();
// }
//
// QString StyleElementQml::subElementName() const
// {
//     return m_subElementName;
// }
//
// void StyleElementQml::setSubElementName(const QString & newSubElementName)
// {
//     if (newSubElementName == m_subElementName) {
//         return;
//     }
//
//     m_subElementName = newSubElementName;
//     updateElement();
//     Q_EMIT subElementNameChanged();
// }
//
void Element::updateElement()
{
    ElementIdentifier identifier{QStringLiteral("plasmasvg"), m_name, QStringLiteral("normal")};

    auto element = Style::instance()->get(identifier);
    m_normalElement = element;

    identifier.setSubElement(QStringLiteral("hovered"));
    element = Style::instance()->get(identifier);
    m_hoverElement = element;

    identifier.setSubElement(QStringLiteral("focus"));
    element = Style::instance()->get(identifier);
    m_focusElement = element;

    identifier.setSubElement(QStringLiteral("activeFocus"));
    element = Style::instance()->get(identifier);
    m_activeFocusElement = element;

    identifier.setSubElement(QStringLiteral("pressed"));
    element = Style::instance()->get(identifier);
    m_pressElement = element;

    Q_EMIT stateChanged();
}
