/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Element.h"

#include "ElementIdentifier.h"
#include "Style.h"

using namespace Union;

Element::Element(QObject *parent)
    : QObject(parent)
{
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
    ElementIdentifier identifier{QStringLiteral("plasma-svg"), m_name, QStringLiteral("normal")};

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
