// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "StyleUtils.h"
#include <ElementQuery.h>
#include <StyleRegistry.h>

#include <QStyleOption>

Union::Element::States buttonStatesFromOption(const QStyleOption *option)
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
    if (option->state.testFlag(QStyle::State_Sunken)) {
        states |= Union::Element::State::Pressed;
    }
    return states;
}

QRectF prepareRectangle(const QStyleOption *option, const Union::Properties::StyleProperty *properties, const QMarginsF &adjustments)
{
    // Shrink the widget rect by the insets
    // TODO: we may have to keep it as a QRectF here
    QRect rect = option->rect;
    if (const auto layout = properties->layout()) {
        if (layout->inset()) {
            rect -= layout->inset()->toMargins().toMargins();
        }
    }

    // Make sure to take out the space left for the visual focus rect
    rect.setLeft(rect.left() + adjustments.left());
    rect.setRight(rect.right() - adjustments.right());
    rect.setTop(rect.top() + adjustments.top());
    rect.setBottom(rect.bottom() - adjustments.bottom());

    return rect;
}

Union::Properties::StyleProperty *prepareProperties(Union::Element::Ptr &element)
{
    const auto style = Union::StyleRegistry::instance()->defaultStyle();
    const auto query = std::make_unique<Union::ElementQuery>(style);

    query->setElements({element});
    query->execute();

    auto properties = query->properties();
    return properties;
}
