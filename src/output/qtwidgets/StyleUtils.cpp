// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "StyleUtils.h"
#include <ElementQuery.h>
#include <StyleRegistry.h>

#include <QStyleOption>

Union::Element::States statesFromOption(const QStyleOption *option)
{
    Union::Element::States states;
    if (option->state.testFlag(QStyle::State_None)) {
        return states;
    }
    if (option->state.testFlag(QStyle::State_MouseOver)) {
        states.setFlag(Union::Element::State::Hovered);
    }
    if (option->state.testFlag(QStyle::State_HasFocus)) {
        states.setFlag(Union::Element::State::ActiveFocus);
    }
    if (!option->state.testFlag(QStyle::State_Enabled)) {
        states.setFlag(Union::Element::State::Disabled);
    }
    if (option->state.testFlag(QStyle::State_On)) {
        states.setFlag(Union::Element::State::Checked);
    }
    if (option->state.testFlag(QStyle::State_Off)) {
        states.setFlag(Union::Element::State::Checked, false);
    }
    if (option->state.testFlag(QStyle::State_Sunken)) {
        states.setFlag(Union::Element::State::Pressed);
    }
    if (option->state.testFlag(QStyle::State_Raised)) {
        states.setFlag(Union::Element::State::Pressed, false);
    }
    if (option->state.testFlag(QStyle::State_Selected)) {
        states.setFlag(Union::Element::State::VisualFocus);
    }

    return states;
}

QStringList hintsFromOption(const QStyleOption *option)
{
    QStringList hints;
    switch ((QStyleOption::OptionType)option->type) {
    case QStyleOption::SO_FocusRect: {
        const auto optionFocusRect = static_cast<const QStyleOptionFocusRect *>(option);
        if (optionFocusRect->state.testFlag(QStyle::State_FocusAtBorder)) {
            hints.append(QStringLiteral("focus-at-border"));
        }
    } break;
    case QStyleOption::SO_Button: {
        const auto optionButton = static_cast<const QStyleOptionButton *>(option);
        if (optionButton->features.testFlag(QStyleOptionButton::ButtonFeature::None)) {
            return hints;
        }
        if (optionButton->features.testFlag(QStyleOptionButton::ButtonFeature::Flat)) {
            hints.append(QStringLiteral("flat"));
        }
        if (optionButton->features.testFlag(QStyleOptionButton::ButtonFeature::HasMenu)) {
            hints.append(QStringLiteral("with-menu"));
        }
        if (optionButton->features.testFlag(QStyleOptionButton::ButtonFeature::DefaultButton)) {
            hints.append(QStringLiteral("default-button"));
        }
        if (optionButton->features.testFlag(QStyleOptionButton::ButtonFeature::AutoDefaultButton)) {
            hints.append(QStringLiteral("auto-default-button"));
        }
        if (optionButton->features.testFlag(QStyleOptionButton::ButtonFeature::CommandLinkButton)) {
            hints.append(QStringLiteral("command-link-button"));
        }
        if (!optionButton->state.testFlag(QStyle::State_AutoRaise)) {
            hints.append(QStringLiteral("raised"));
        }
    } break;
    case QStyleOption::SO_Tab:
    case QStyleOption::SO_MenuItem:
    case QStyleOption::SO_Frame:
    case QStyleOption::SO_ProgressBar:
    case QStyleOption::SO_ToolBox:
    case QStyleOption::SO_Header:
    case QStyleOption::SO_DockWidget:
    case QStyleOption::SO_ViewItem:
    case QStyleOption::SO_TabWidgetFrame:
    case QStyleOption::SO_TabBarBase:
    case QStyleOption::SO_RubberBand:
    case QStyleOption::SO_ToolBar:
    case QStyleOption::SO_GraphicsItem:
    case QStyleOption::SO_Complex:
    case QStyleOption::SO_Slider:
    case QStyleOption::SO_SpinBox:
    case QStyleOption::SO_ToolButton:
    case QStyleOption::SO_ComboBox:
    case QStyleOption::SO_TitleBar:
    case QStyleOption::SO_GroupBox:
    case QStyleOption::SO_SizeGrip:
    default:
        return QStringList();
    }

    return hints;
}

Union::Element::ColorSet colorsetFromOption(const QStyleOption *option)
{
    /*
     *     enum class ColorSet {
     *  None,
     *  View,
     *  Window,
     *  Button,
     *  Selection,
     *  Tooltip,
     *  Complementary,
     *  Header,
    };*/
    switch ((QStyleOption::OptionType)option->type) {
    case QStyleOption::SO_Default:
    case QStyleOption::SO_FocusRect:
    case QStyleOption::SO_Tab:
    case QStyleOption::SO_MenuItem:
    case QStyleOption::SO_Frame:
    case QStyleOption::SO_ProgressBar:
    case QStyleOption::SO_ToolBox:
    case QStyleOption::SO_Header:
    case QStyleOption::SO_DockWidget:
    case QStyleOption::SO_ViewItem:
    case QStyleOption::SO_TabWidgetFrame:
    case QStyleOption::SO_TabBarBase:
    case QStyleOption::SO_GraphicsItem:
    case QStyleOption::SO_ToolBar:
    case QStyleOption::SO_Complex:
    case QStyleOption::SO_Slider:
    case QStyleOption::SO_SpinBox:
    case QStyleOption::SO_ComboBox:
    case QStyleOption::SO_TitleBar:
    case QStyleOption::SO_SizeGrip:
        return Union::Element::ColorSet::None;
    case QStyleOption::SO_Button:
    case QStyleOption::SO_ToolButton:
        return Union::Element::ColorSet::Button;
    case QStyleOption::SO_RubberBand:
        return Union::Element::ColorSet::Selection;
    case QStyleOption::SO_GroupBox:
        return Union::Element::ColorSet::Complementary;
    default:
        return Union::Element::ColorSet::None;
    }
}

QVariantMap attributesFromOption(const QStyleOption *option)
{
    return QVariantMap();
}

QRectF prepareRectangle(const QStyleOption *option, const Union::Properties::StylePropertyGroup *properties, const QMarginsF &adjustments)
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

Union::Properties::StylePropertyGroup *prepareProperties(Union::Element::Ptr &element)
{
    const auto style = Union::StyleRegistry::instance()->defaultStyle();
    const auto query = std::make_unique<Union::ElementQuery>(style);

    query->setElements({element});
    query->execute();

    auto properties = query->properties();
    return properties;
}
