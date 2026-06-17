// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "StyleUtils.h"
#include <ElementQuery.h>
#include <StyleRegistry.h>

#include <QStyleOption>
#include <QStyleOptionFrame>
#include <QTableView>

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
        states.setFlag(Union::Element::State::Highlighted);
    }

    return states;
}

QStringList hintsFromOption(const QStyleOption *option)
{
    QStringList hints;
    switch ((QStyleOption::OptionType)option->type) {
    case QStyleOption::SO_FocusRect: {
        if (const auto optionFocusRect = static_cast<const QStyleOptionFocusRect *>(option)) {
            if (optionFocusRect->state.testFlag(QStyle::State_FocusAtBorder)) {
                hints.append(QStringLiteral("focus-at-border"));
            }
        }
    } break;
    case QStyleOption::SO_Button: {
        if (const auto optionButton = static_cast<const QStyleOptionButton *>(option)) {
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
        }
    } break;
    case QStyleOption::SO_ViewItem: {
        if (const auto optionViewItem = static_cast<const QStyleOptionViewItem *>(option)) {
            auto viewItemPosition = optionViewItem->viewItemPosition;
            const auto table = qobject_cast<const QTableView *>(optionViewItem->widget);

            // For tables and such, we just want to select one item.
            if (table) {
                viewItemPosition = QStyleOptionViewItem::Invalid;
            }

            switch (viewItemPosition) {
            case QStyleOptionViewItem::Invalid:
                hints.append(QStringLiteral("position-invalid"));
                break;
            case QStyleOptionViewItem::Beginning:
                hints.append(QStringLiteral("position-beginning"));
                break;
            case QStyleOptionViewItem::Middle:
                hints.append(QStringLiteral("position-middle"));
                break;
            case QStyleOptionViewItem::End:
                hints.append(QStringLiteral("position-end"));
                break;
            case QStyleOptionViewItem::OnlyOne:
                hints.append(QStringLiteral("position-onlyone"));
                break;
            }
        }
    } break;
    case QStyleOption::SO_Frame: {
        if (const auto optionFrame = static_cast<const QStyleOptionFrame *>(option)) {
            if (optionFrame->features.testFlag(QStyleOptionFrame::Flat)) {
                hints.append(QStringLiteral("flat"));
            }
            if (optionFrame->features.testFlag(QStyleOptionFrame::Rounded)) {
                hints.append(QStringLiteral("rounded"));
            }
            switch (optionFrame->frameShape) {
            case QFrame::NoFrame:
            case QFrame::Box:
            case QFrame::Panel:
            case QFrame::WinPanel:
            case QFrame::HLine:
            case QFrame::VLine:
            case QFrame::StyledPanel:
                break;
            }
        }
    } break;

    case QStyleOption::SO_Tab:
    case QStyleOption::SO_MenuItem:
    case QStyleOption::SO_ProgressBar:
    case QStyleOption::SO_ToolBox:
    case QStyleOption::SO_Header:
    case QStyleOption::SO_DockWidget:
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
    switch ((QStyleOption::OptionType)option->type) {
    case QStyleOption::SO_ViewItem:
    case QStyleOption::SO_Default:
    case QStyleOption::SO_FocusRect:
    case QStyleOption::SO_Button:
    case QStyleOption::SO_Tab:
    case QStyleOption::SO_MenuItem:
    case QStyleOption::SO_Frame:
    case QStyleOption::SO_ProgressBar:
    case QStyleOption::SO_ToolBox:
    case QStyleOption::SO_Header:
    case QStyleOption::SO_DockWidget:
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
    case QStyleOption::SO_CustomBase:
    case QStyleOption::SO_ComplexCustomBase:
        break;
    }
    return QVariantMap();
}

QRectF prepareRectangle(const QStyleOption *option, const Union::Properties::StylePropertyGroup *properties)
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
    // Button and ComboBox types have focus rect so we need to reserve room for them
    if (option->type == QStyleOption::OptionType::SO_Button || option->type == QStyleOption::OptionType::SO_ComboBox) {
        // TODO: get the size from the focusRect subelement
        QMarginsF adjustments(2, 2, 2, 2);
        rect.setLeft(rect.left() + adjustments.left());
        rect.setRight(rect.right() - adjustments.right());
        rect.setTop(rect.top() + adjustments.top());
        rect.setBottom(rect.bottom() - adjustments.bottom());
    }

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

QStringList setupMemberList(QWidget *widget)
{
    if (!widget) {
        return QStringList();
    }
    QStringList members;
    // We will have to check what items the widget inherits from,
    // as far as I know there is no better way to do this.
    const QMap<const char *, QString> parentClasses = {
        {"QCheckBox", QStringLiteral("CheckBox")},
        {"QRadioButton", QStringLiteral("RadioButton")},
        {"QPushButton", QStringLiteral("Button")},
        {"QToolButton", QStringLiteral("ToolButton")},
        {"QDial", QStringLiteral("Dial")},
        {"QScrollBar", QStringLiteral("ScrollBar")},
        {"QSlider", QStringLiteral("Slider")},
        {"QAbstractSpinox", QStringLiteral("SpinBox")},
        {"QComboBox", QStringLiteral("ComboBox")},
        {"QDialog", QStringLiteral("Dialog")},
        {"QDialogButtonox", QStringLiteral("DialogButtonBox")},
        {"QDockWidget", QStringLiteral("Dock")},
        {"QFocusFrame", QStringLiteral("FocusFrame")},
        {"QFrame", QStringLiteral("Frame")},
        {"QGroupBox", QStringLiteral("GroupBox")},
        {"QKeySequenceEit", QStringLiteral("KeySequenceEdit")},
        {"QLineEdit", QStringLiteral("TextField")},
        {"QMainWindow", QStringLiteral("ApplicationWindow")},
        {"QMdiSubWinow", QStringLiteral("MdiSubWindow")},
        {"QMenu", QStringLiteral("Menu")},
        {"QMenuBar", QStringLiteral("MenuBar")},
        {"QProgressBar", QStringLiteral("ProgressBar")},
        {"QRubberBand", QStringLiteral("RubberBand")},
        {"QSizeGrip", QStringLiteral("SizeGrip")},
        {"QSplitterHandle", QStringLiteral("SplitterHandle")},
        {"QStatusBar", QStringLiteral("StatusBar")},
        {"QTabBar", QStringLiteral("TabBar")},
        {"QTabWidget", QStringLiteral("TabWidget")},
        {"QToolBar", QStringLiteral("ToolBar")},
    };

    auto currentWidget = widget;
    while (currentWidget) {
        for (const auto classes : parentClasses.asKeyValueRange()) {
            if (currentWidget->inherits(classes.first)) {
                members.prepend(classes.second);
                break;
            }
        }
        currentWidget = currentWidget->parentWidget();
    }

    return members;
}
