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
    if (!option) {
        return states;
    }
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
    if (!option) {
        return hints;
    }
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

    case QStyleOption::SO_ToolButton: {
        if (const auto optionButton = static_cast<const QStyleOptionToolButton *>(option)) {
            if (optionButton->features.testFlag(QStyleOptionToolButton::ToolButtonFeature::None)) {
                return hints;
            }
            if (optionButton->features.testFlag(QStyleOptionToolButton::ToolButtonFeature::HasMenu)) {
                hints.append(QStringLiteral("with-menu"));
            }
            if (!optionButton->state.testFlag(QStyle::State_AutoRaise)) {
                hints.append(QStringLiteral("raised"));
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
    if (!option) {
        return Union::Element::ColorSet::None;
    }
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
    if (!option) {
        return QVariantMap();
    }
    switch ((QStyleOption::OptionType)option->type) {
    case QStyleOption::SO_ToolButton:
        if (const auto optionButton = static_cast<const QStyleOptionToolButton *>(option)) {
            QVariantMap map;
            switch (optionButton->toolButtonStyle) {
            case Qt::ToolButtonIconOnly:
                map[QStringLiteral("display")] = QVariant(QStringLiteral("icon-only"));
                break;
            case Qt::ToolButtonTextOnly:
                map[QStringLiteral("display")] = QVariant(QStringLiteral("text-only"));
                break;
            case Qt::ToolButtonTextBesideIcon:
                map[QStringLiteral("display")] = QVariant(QStringLiteral("text-beside-icon"));
                break;
            case Qt::ToolButtonTextUnderIcon:
                map[QStringLiteral("display")] = QVariant(QStringLiteral("text-under-icon"));
                break;
            default:
                return map;
            }
            return map;
        }
        break;
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

Qt::Alignment toQtAlignment(Union::Properties::AlignmentPropertyGroup *alignmentGroup)
{
    Qt::Alignment verticalAlignment = Qt::AlignVCenter;
    Qt::Alignment horizontalAlignment = Qt::AlignLeft;

    if (!alignmentGroup) {
        return verticalAlignment | horizontalAlignment;
    }

    auto unionVertical = alignmentGroup->vertical().value_or(Union::Properties::Alignment::Unspecified);
    auto unionHorizontal = alignmentGroup->horizontal().value_or(Union::Properties::Alignment::Unspecified);

    switch (unionVertical) {
    case Union::Properties::Alignment::Unspecified:
    case Union::Properties::Alignment::Fill:
    case Union::Properties::Alignment::StackCenter:
    case Union::Properties::Alignment::StackFill:
    case Union::Properties::Alignment::Center:
        verticalAlignment = Qt::AlignVCenter;
        break;
    case Union::Properties::Alignment::Start:
        verticalAlignment = Qt::AlignTop;
        break;
    case Union::Properties::Alignment::End:
        verticalAlignment = Qt::AlignBottom;
        break;
    }

    switch (unionHorizontal) {
    case Union::Properties::Alignment::Unspecified:
    case Union::Properties::Alignment::Fill:
    case Union::Properties::Alignment::StackCenter:
    case Union::Properties::Alignment::StackFill:
    case Union::Properties::Alignment::Start:
        horizontalAlignment = Qt::AlignLeft;
        break;
    case Union::Properties::Alignment::Center:
        horizontalAlignment = Qt::AlignHCenter;
        break;
    case Union::Properties::Alignment::End:
        horizontalAlignment = Qt::AlignRight;
        break;
    }

    return verticalAlignment | horizontalAlignment;
}

Qt::TextElideMode toQtElideMode(Union::Properties::TextElide elideMode)
{
    Qt::TextElideMode elide;
    switch (elideMode) {
    case Union::Properties::TextElide::None:
        elide = Qt::TextElideMode::ElideNone;
        break;
    case Union::Properties::TextElide::Left:
        elide = Qt::TextElideMode::ElideLeft;
        break;
    case Union::Properties::TextElide::Middle:
        elide = Qt::TextElideMode::ElideMiddle;
        break;
    case Union::Properties::TextElide::Right:
        elide = Qt::TextElideMode::ElideRight;
        break;
    }
    return elide;
}
Qt::TextFlag toQtWrapMode(Union::Properties::TextWrapMode wrapMode)
{
    Qt::TextFlag wrap = Qt::TextFlag::TextDontClip;
    switch (wrapMode) {
    case Union::Properties::TextWrapMode::NoWrap:
    case Union::Properties::TextWrapMode::ManualWrap:
        break;
    case Union::Properties::TextWrapMode::WordWrap:
    case Union::Properties::TextWrapMode::WrapAtWordBoundaryOrAnywhere:
        wrap = Qt::TextFlag::TextWordWrap;
        break;
    case Union::Properties::TextWrapMode::WrapAnywhere:
        wrap = Qt::TextFlag::TextWrapAnywhere;
        break;
    }
    return wrap;
}

QRectF backgroundRectangle(const QStyleOption *option, const Union::Properties::StylePropertyGroup *properties)
{
    // Shrink the widget rect by the insets
    QRectF rect = option->rect;
    if (const auto layout = properties->layout()) {
        if (layout->inset()) {
            rect -= layout->inset()->toMargins();
        }
    }
    return rect;
}

Union::ElementList prepareElements(const QStyleOption *opt, const QWidget *widget, QStringList childElementNames)
{
    Union::ElementList elements;
    QStringList elementTypes;

    if (widget) {
        elementTypes = widget->property(property_union_member_list).toStringList();
        if (elementTypes.isEmpty()) {
            elementTypes = setupMemberList(widget);
        }
    }
    if (!childElementNames.isEmpty()) {
        elementTypes.append(childElementNames);
    }

    for (const auto &elementType : elementTypes) {
        auto unionElement = Union::Element::create();
        unionElement->setType(elementType);
        unionElement->setStates(statesFromOption(opt));
        unionElement->setHints(hintsFromOption(opt));
        unionElement->setColorSet(colorsetFromOption(opt));
        unionElement->setAttributes(attributesFromOption(opt));
        elements.append(unionElement);
    }
    return elements;
}

Union::Properties::StylePropertyGroup *queryProperties(const Union::ElementList &elements)
{
    const auto style = Union::StyleRegistry::instance()->defaultStyle();
    const auto query = std::make_unique<Union::ElementQuery>(style);

    query->setElements(elements);
    query->execute();
    auto properties = query->properties();

    return properties;
}

QStringList setupMemberList(const QWidget *widget)
{
    if (!widget) {
        return QStringList();
    }
    QStringList members;
    // We will have to check what items the widget inherits from,
    // as far as I know there is no better way to do this.
    const QMap<const char *, QString> parentClasses = {{"QCheckBox", QStringLiteral("CheckBox")},
                                                       {"QRadioButton", QStringLiteral("RadioButton")},
                                                       {"QPushButton", QStringLiteral("Button")},
                                                       {"QToolButton", QStringLiteral("ToolButton")},
                                                       {"QDial", QStringLiteral("Dial")},
                                                       {"QScrollBar", QStringLiteral("ScrollBar")},
                                                       {"QSlider", QStringLiteral("Slider")},
                                                       {"QAbstractSpinBox", QStringLiteral("SpinBox")},
                                                       {"QComboBox", QStringLiteral("ComboBox")},
                                                       {"QDialog", QStringLiteral("Dialog")},
                                                       {"QDialogButtonBox", QStringLiteral("DialogButtonBox")},
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
                                                       {"QAbstractScrollArea", QStringLiteral("ScrollArea")},
                                                       {"QListView", QStringLiteral("ListView")},
                                                       {"QScrollBar", QStringLiteral("ScrollBar")},
                                                       {"QTreeView", QStringLiteral("QTreeViewDelegate")}};

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

QMap<QString, LayoutItem> layoutMap(const Union::ElementList &elements, const QStyleOption *opt, const QStringList &subElements)
{
    QMap<QString, LayoutItem> map;
    QList<LayoutItem> items;

    if (subElements.empty()) {
        qWarning() << "No sublements given, returning empty map!";
        return map;
    }

    // TODO: Go through all elements, create rectangles for them
    // Then place and resize those rectangles according to hierarchy
    // Use the original opt->rect as the main container
    // move any subelements in it according their given rules
    QRectF availableSpace = opt->rect;
    // Get spacing for main item
    auto properties = queryProperties(elements);
    int spacing = 0;
    if (subElements.count() > 1) {
        spacing = properties->layout()->spacing().value_or(0);
    }
    auto currentHierarchy = elements;
    for (const auto &subElement : subElements) {
        // NOTE: Currently text and icon are part of the main element, but eventually
        // will be moved as their own elements
        if (subElement != QStringLiteral("Icon") && subElement != QStringLiteral("Text")) {
            auto unionElement = Union::Element::create();
            unionElement->setType(subElement);
            unionElement->setStates(statesFromOption(opt));
            unionElement->setHints(hintsFromOption(opt));
            unionElement->setColorSet(colorsetFromOption(opt));
            unionElement->setAttributes(attributesFromOption(opt));
            currentHierarchy.append(unionElement);
        }
        properties = queryProperties(currentHierarchy);
        Union::Properties::Alignment horizontalAlignment;
        Union::Properties::Alignment verticalAlignment;
        int order = 0;
        QRectF elementRect = availableSpace;
        // NOTE: For now icon and text are their own things, so check them separately.
        // in future this should be unnecessary.
        if (subElement == QStringLiteral("Icon")) {
            elementRect.setWidth(properties->icon()->width().value_or(0));
            elementRect.setHeight(properties->icon()->height().value_or(0));
            horizontalAlignment = properties->icon()->alignment()->horizontal().value_or(Union::Properties::Alignment::Unspecified);
            verticalAlignment = properties->icon()->alignment()->vertical().value_or(Union::Properties::Alignment::Unspecified);
            order = properties->icon()->alignment()->order().value_or(0);
        } else if (subElement == QStringLiteral("Text")) {
            horizontalAlignment = properties->text()->alignment()->horizontal().value_or(Union::Properties::Alignment::Unspecified);
            verticalAlignment = properties->text()->alignment()->vertical().value_or(Union::Properties::Alignment::Unspecified);
            if (const auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
                elementRect = opt->fontMetrics.boundingRect(buttonOption->text);
            } else if (const auto buttonOption = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
                elementRect = opt->fontMetrics.boundingRect(buttonOption->text);
            }
            order = properties->text()->alignment()->order().value_or(0);
        } else {
            elementRect.setWidth(properties->layout()->width().value_or(0));
            elementRect.setHeight(properties->layout()->height().value_or(0));
            horizontalAlignment = properties->layout()->alignment()->horizontal().value_or(Union::Properties::Alignment::Unspecified);
            verticalAlignment = properties->layout()->alignment()->vertical().value_or(Union::Properties::Alignment::Unspecified);
            order = properties->layout()->alignment()->order().value_or(0);
        }
        LayoutItem item = LayoutItem();
        item.elementName = subElement;
        item.horizontalAlignment = horizontalAlignment;
        item.verticalAlignment = verticalAlignment;
        item.order = order;
        item.rect = elementRect;
        items.append(item);
    }

    // Sort the list according to order
    std::sort(items.begin(), items.end(), [](const LayoutItem &lhs, const LayoutItem &rhs) {
        return lhs.order < rhs.order;
    });

    // QtWidgets containment is always within Widget, since we can't draw outside of a widget due
    // widgets limitations.
    QRectF previousRect;
    for (auto &item : items) {
        switch (item.horizontalAlignment) {
        case Union::Properties::Alignment::Unspecified:
        case Union::Properties::Alignment::StackFill:
        case Union::Properties::Alignment::StackCenter:
        case Union::Properties::Alignment::Start:
            if (!previousRect.isEmpty()) {
                item.rect.moveLeft(previousRect.right());
                item.rect.adjust(spacing, 0, spacing, 0);
            } else {
                item.rect.moveLeft(availableSpace.left());
            }
            availableSpace.moveLeft(item.rect.right());
            break;
        case Union::Properties::Alignment::Fill:
            if (!previousRect.isEmpty()) {
                item.rect.moveLeft(previousRect.right());
                item.rect.moveRight(availableSpace.right());
                item.rect.adjust(spacing, 0, spacing, 0);
            } else {
                item.rect.moveRight(availableSpace.right());
                item.rect.moveLeft(availableSpace.left());
            }
            availableSpace.moveLeft(item.rect.right());
            break;
        case Union::Properties::Alignment::Center:
            // For single items and vertical stackCenter/stackFill,
            // we can just utilize the exact center,
            // since we do not need to move other items around
            if (items.size() > 1 && item.verticalAlignment != Union::Properties::Alignment::StackCenter
                && item.verticalAlignment != Union::Properties::Alignment::StackFill) {
                if (!previousRect.isEmpty()) {
                    item.rect.moveLeft(previousRect.right());
                    item.rect.adjust(spacing, 0, spacing, 0);
                } else {
                    item.rect.moveLeft(availableSpace.left());
                }
                availableSpace.moveLeft(item.rect.right());
            } else {
                item.rect.moveCenter(QPoint(availableSpace.center().x(), item.rect.center().y()));
            }
            break;
        case Union::Properties::Alignment::End:
            if (!previousRect.isEmpty()) {
                item.rect.moveRight(previousRect.left());
                item.rect.adjust(spacing, 0, spacing, 0);
            } else {
                item.rect.moveRight(availableSpace.right());
            }
            availableSpace.moveRight(item.rect.left());
            break;
        }
        switch (item.verticalAlignment) {
        case Union::Properties::Alignment::Unspecified:
        case Union::Properties::Alignment::Start:
            if (!previousRect.isEmpty()) {
                item.rect.moveTop(previousRect.bottom());
                item.rect.adjust(0, spacing, 0, spacing);
            } else {
                item.rect.moveCenter(QPoint(item.rect.center().x(), availableSpace.top()));
            }
            availableSpace.moveTop(item.rect.bottom());
            break;
        case Union::Properties::Alignment::Fill:
            item.rect.moveTop(availableSpace.top());
            item.rect.moveBottom(availableSpace.bottom());
            break;
        case Union::Properties::Alignment::Center:
            item.rect.moveCenter(QPoint(item.rect.center().x(), availableSpace.center().y()));
            break;
        case Union::Properties::Alignment::End:
            if (!previousRect.isEmpty()) {
                item.rect.moveTop(previousRect.bottom());
                item.rect.adjust(0, spacing, 0, spacing);
            } else {
                item.rect.moveCenter(QPoint(item.rect.center().x(), availableSpace.bottom()));
            }
            availableSpace.moveTop(item.rect.bottom());
            break;
        case Union::Properties::Alignment::StackFill:
            if (!previousRect.isEmpty()) {
                item.rect.moveTop(previousRect.bottom());
                item.rect.adjust(0, spacing, 0, spacing);
                item.rect.moveBottom(availableSpace.bottom());
            } else {
                item.rect.moveTop(availableSpace.top());
                item.rect.moveBottom(availableSpace.bottom());
            }
        case Union::Properties::Alignment::StackCenter:
            if (!previousRect.isEmpty()) {
                item.rect.moveTop(previousRect.bottom());
                item.rect.adjust(0, spacing, 0, spacing);
            } else {
                item.rect.moveTop(availableSpace.top());
            }
            availableSpace.moveTop(item.rect.bottom());
            break;
        }

        // QtWidgets does not allow drawing outside of the
        // widget area, so constrain it.
        if (item.rect.x() < 0) {
            item.rect.setX(0);
        }
        if (item.rect.y() < 0) {
            item.rect.setY(0);
        }

        previousRect = item.rect;
        map[item.elementName] = item;
    }
    return map;
}