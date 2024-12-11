// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "StyleUtils.h"

#include <QStyleOption>

Union::Element::States statesFromOption(const QStyleOption *option)
{
    Union::Element::States states;
    if (option->state.testFlag(QStyle::State_Active)) {
        states |= Union::Element::State::ActiveFocus;
    }
    if (option->state.testFlag(QStyle::State_MouseOver)) {
        states |= Union::Element::State::Hovered;
    }
    if (option->state.testFlag(QStyle::State_HasFocus)) {
        states |= Union::Element::State::VisualFocus;
    }
    if (!option->state.testFlag(QStyle::State_Enabled)) {
        states |= Union::Element::State::Disabled;
    }
    if (option->state.testFlag(QStyle::State_On)) {
        states |= Union::Element::State::Checked;
    }
    if (!option->state.testFlag(QStyle::State_Raised)) {
        states |= Union::Element::State::Pressed;
    }
    return states;
}
