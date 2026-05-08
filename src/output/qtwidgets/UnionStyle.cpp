// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "UnionStyle.h"
#include "StyleDrawing.h"
#include "StyleUtils.h"

#include <ElementQuery.h>
#include <QApplication>
#include <StyleRegistry.h>

#include <QPainter>
#include <QPushButton>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleOption>
#include <QWidget>

UnionStyle::UnionStyle()
    : QProxyStyle(qEnvironmentVariable("UNION_WIDGETS_BASE_STYLE", QStringLiteral("breeze")))
{
    Union::StyleRegistry::instance()->load();
}

void UnionStyle::drawControl(QStyle::ControlElement controlElement, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // Make lines not look completely terrible on fractional scales
    painter->setRenderHint(QPainter::Antialiasing, true);

    switch (controlElement) {
    case QStyle::CE_PushButton: {
        const auto buttonOption = static_cast<const QStyleOptionButton *>(option);
        drawElement(queryProperties(prepareElements(option, widget)), painter, option);
        drawControl(CE_PushButtonLabel, buttonOption, painter, widget);
    }
        return;
    case QStyle::CE_CheckBoxLabel:
    case QStyle::CE_PushButtonLabel: {
        const auto buttonOption = static_cast<const QStyleOptionButton *>(option);
        drawIconText(buttonOption, this, painter, widget, buttonOption->icon, buttonOption->text);
    }
        return;
    case QStyle::CE_ToolButtonLabel: {
        const auto buttonOption = static_cast<const QStyleOptionToolButton *>(option);
        auto text = buttonOption->text;
        // Skip text drawing for icon only buttons completely
        if (buttonOption->toolButtonStyle == Qt::ToolButtonIconOnly) {
            text = QString();
        }
        drawIconText(buttonOption, this, painter, widget, buttonOption->icon, text);
    }
        return;
    case QStyle::CE_CheckBox: {
        const auto buttonOption = static_cast<const QStyleOptionButton *>(option);

        auto bgElements = prepareElements(option, widget, {QStringLiteral("CheckBox")});
        auto bgProps = queryProperties(bgElements);
        auto rect = backgroundRectangle(option, bgProps).toRect();
        drawBackground(painter, rect, bgProps);

        // TODO this probably could be cleaner
        auto indicatorElements = prepareElements(option, widget, {QStringLiteral("CheckBox"), QStringLiteral("Indicator")});
        auto indicatorProps = queryProperties(indicatorElements);
        auto indicatorMap = layoutMap(buttonOption->rect, indicatorElements, option, {QStringLiteral("Indicator"), QStringLiteral("Text")});
        drawElement(indicatorProps, painter, option, indicatorMap[QStringLiteral("Indicator")].toRect());

        auto textRect = indicatorMap[QStringLiteral("Text")].toRect();
        QColor textColor = bgProps->text()->color().value().toQColor();
        painter->setPen(textColor);
        drawItemText(painter,
                     textRect,
                     Qt::TextShowMnemonic | toQtAlignment(bgProps->text()->alignment()),
                     buttonOption->palette,
                     buttonOption->state & State_Enabled,
                     buttonOption->text,
                     textColor.isValid() ? QPalette::NoRole : QPalette::WindowText);
    }
        return;

    case QStyle::CE_PushButtonBevel:
    case QStyle::CE_RadioButton:
    case QStyle::CE_RadioButtonLabel:
    case QStyle::CE_TabBarTab:
    case QStyle::CE_TabBarTabShape:
    case QStyle::CE_TabBarTabLabel:
    case QStyle::CE_ProgressBar:
    case QStyle::CE_ProgressBarGroove:
    case QStyle::CE_ProgressBarContents:
    case QStyle::CE_ProgressBarLabel:
    case QStyle::CE_MenuItem:
    case QStyle::CE_MenuScroller:
    case QStyle::CE_MenuVMargin:
    case QStyle::CE_MenuHMargin:
    case QStyle::CE_MenuTearoff:
    case QStyle::CE_MenuEmptyArea:
    case QStyle::CE_MenuBarItem:
    case QStyle::CE_MenuBarEmptyArea:
    case QStyle::CE_Header:
    case QStyle::CE_HeaderSection:
    case QStyle::CE_HeaderLabel:
    case QStyle::CE_ToolBoxTab:
    case QStyle::CE_SizeGrip:
    case QStyle::CE_Splitter:
    case QStyle::CE_RubberBand:
    case QStyle::CE_DockWidgetTitle:
    case QStyle::CE_ScrollBarAddLine:
    case QStyle::CE_ScrollBarSubLine:
    case QStyle::CE_ScrollBarAddPage:
    case QStyle::CE_ScrollBarSubPage:
    case QStyle::CE_ScrollBarSlider:
    case QStyle::CE_ScrollBarFirst:
    case QStyle::CE_ScrollBarLast:
    case QStyle::CE_FocusFrame:
    case QStyle::CE_ComboBoxLabel:
    case QStyle::CE_ToolBar:
    case QStyle::CE_ToolBoxTabShape:
    case QStyle::CE_ToolBoxTabLabel:
    case QStyle::CE_HeaderEmptyArea:
    case QStyle::CE_ColumnViewGrip:
    case QStyle::CE_ItemViewItem:
    case QStyle::CE_ShapedFrame:
    case QStyle::CE_CustomBase:
        break;
    }

    QProxyStyle::drawControl(controlElement, option, painter, widget);
}

// Complex controls are bit annoying. We may need to manually handle some things to make sure they work correctly
void UnionStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    switch (control) {
    case QStyle::CC_ToolButton: {
        const auto buttonOption = static_cast<const QStyleOptionToolButton *>(option);
        drawElement(queryProperties(prepareElements(option, widget)), painter, buttonOption);
        drawControl(CE_ToolButtonLabel, buttonOption, painter, widget);
    }
        return;
    case QStyle::CC_GroupBox: {
        const auto groupBoxOption = static_cast<const QStyleOptionGroupBox *>(option);
        const auto elements = prepareElements(groupBoxOption, widget);
        const auto properties = queryProperties(elements);
        auto rect = backgroundRectangle(groupBoxOption, properties).toRect();
        drawBackground(painter, rect, properties);
        if ((groupBoxOption->subControls & QStyle::SC_GroupBoxLabel) && !groupBoxOption->text.isEmpty()) {
            QRect textRect = subControlRect(CC_GroupBox, option, SC_GroupBoxLabel, widget);
            QColor textColor = properties->text()->color().value().toQColor();

            painter->setPen(textColor);
            drawItemText(painter,
                         textRect,
                         Qt::TextShowMnemonic | groupBoxOption->textAlignment,
                         groupBoxOption->palette,
                         groupBoxOption->state & State_Enabled,
                         groupBoxOption->text,
                         textColor.isValid() ? QPalette::NoRole : QPalette::WindowText);
        }
    }
        return;
    case QStyle::CC_SpinBox:
    case QStyle::CC_ComboBox:
    case QStyle::CC_ScrollBar:
    case QStyle::CC_Slider:
    case QStyle::CC_TitleBar:
    case QStyle::CC_Dial:
    case QStyle::CC_MdiControls:
    case QStyle::CC_CustomBase:
        break;
    }

    QProxyStyle::drawComplexControl(control, option, painter, widget);
}

void UnionStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case QStyle::PE_FrameStatusBarItem:
        drawElement(queryProperties(prepareElements(option, widget, {QStringLiteral("Item")})), painter, option);
        return;
    case QStyle::PE_Widget:
        // Relates to PE_Frame
        drawElement(queryProperties(prepareElements(option, widget, {QStringLiteral("Panel")})), painter, option);
        return;
        // Standalone elements
    case QStyle::PE_PanelLineEdit:
        // For spinboxes and comboboxes, we do not want to draw this element
        // TODO: remove if this is not needed after making the complex controls
        if (widget->parentWidget()->inherits("QComboBox") || widget->parentWidget()->inherits("QAbstractSpinBox")) {
            return;
        }
        drawElement(queryProperties(prepareElements(option, widget)), painter, option);
        return;
    case QStyle::PE_PanelItemViewItem:
        drawElement(queryProperties(prepareElements(option, widget, {QStringLiteral("ItemViewItem")})), painter, option);
        return;
    case QStyle::PE_PanelItemViewRow:
        drawElement(queryProperties(prepareElements(option, widget, {QStringLiteral("ItemViewRow")})), painter, option);
        return;
    case QStyle::PE_PanelScrollAreaCorner:
        drawElement(queryProperties(prepareElements(option, widget, {QStringLiteral("ScrollAreaCorner")})), painter, option);
        return;
    case QStyle::PE_PanelTipLabel:
        drawElement(queryProperties(prepareElements(option, widget, {QStringLiteral("ToolTip")})), painter, option);
        return;
    case QStyle::PE_FrameFocusRect:
        drawElement(queryProperties(prepareElements(option, widget, {QStringLiteral("FocusFrame")})), painter, option);
        return;
        // Indicators
    case QStyle::PE_IndicatorCheckBox:
        drawElement(queryProperties(prepareElements(option, widget, {QStringLiteral("CheckBox"), QStringLiteral("Indicator")})), painter, option);
        return;
    case QStyle::PE_IndicatorRadioButton:
        drawElement(queryProperties(prepareElements(option, widget, {QStringLiteral("RadioButton"), QStringLiteral("Indicator")})), painter, option);
        return;
    case QStyle::PE_IndicatorArrowDown:
    case QStyle::PE_IndicatorArrowLeft:
    case QStyle::PE_IndicatorArrowRight:
    case QStyle::PE_IndicatorArrowUp:
    case QStyle::PE_IndicatorBranch:
    case QStyle::PE_IndicatorButtonDropDown:
    case QStyle::PE_IndicatorItemViewItemCheck:
    case QStyle::PE_IndicatorDockWidgetResizeHandle:
    case QStyle::PE_IndicatorHeaderArrow:
    case QStyle::PE_IndicatorMenuCheckMark:
    case QStyle::PE_IndicatorProgressChunk:
    case QStyle::PE_IndicatorSpinDown:
    case QStyle::PE_IndicatorSpinMinus:
    case QStyle::PE_IndicatorSpinPlus:
    case QStyle::PE_IndicatorSpinUp:
    case QStyle::PE_IndicatorToolBarHandle:
    case QStyle::PE_IndicatorToolBarSeparator:
    case QStyle::PE_IndicatorTabTear:
    case QStyle::PE_IndicatorColumnViewArrow:
    case QStyle::PE_IndicatorItemViewItemDrop:
    case QStyle::PE_IndicatorTabClose:
    case QStyle::PE_IndicatorTabTearRight:
        break;
    default:
        drawElement(queryProperties(prepareElements(option, widget)), painter, option);
        return;
    }

    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

QSize UnionStyle::sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *widget) const
{
    if (ct == CT_PushButton) {
        auto size = QProxyStyle::sizeFromContents(ct, opt, contentsSize, widget);

        auto element = Union::Element::create();
        element->setType(QStringLiteral("Button"));
        element->setColorSet(Union::Element::ColorSet::Button);

        const auto style = Union::StyleRegistry::instance()->defaultStyle();
        const auto matches = style->matches({element});
        const auto properties = matches.first()->properties();

        if (const auto layout = properties->layout()) {
            const QMargins padding = layout->padding()->toMargins().toMargins();
            size = size.grownBy(padding / 2.0f);
        }

        // Make sure to allocate space for the visual focus rect
        size = size.grownBy(QMargins(2, 2, 2, 2));

        return size;
    }
    return QProxyStyle::sizeFromContents(ct, opt, contentsSize, widget);
}

int UnionStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric) {
    // Don't shift button text when sunken
    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
        return 0;
    case QStyle::PM_ButtonMargin:
    case QStyle::PM_ButtonDefaultIndicator:
    case QStyle::PM_MenuButtonIndicator:
    case QStyle::PM_DefaultFrameWidth:
    case QStyle::PM_SpinBoxFrameWidth:
    case QStyle::PM_ComboBoxFrameWidth:
    case QStyle::PM_MaximumDragDistance:
    case QStyle::PM_ScrollBarExtent:
    case QStyle::PM_ScrollBarSliderMin:
    case QStyle::PM_SliderThickness:
    case QStyle::PM_SliderControlThickness:
    case QStyle::PM_SliderLength:
    case QStyle::PM_SliderTickmarkOffset:
    case QStyle::PM_SliderSpaceAvailable:
    case QStyle::PM_DockWidgetSeparatorExtent:
    case QStyle::PM_DockWidgetHandleExtent:
    case QStyle::PM_DockWidgetFrameWidth:
    case QStyle::PM_TabBarTabOverlap:
    case QStyle::PM_TabBarTabHSpace:
    case QStyle::PM_TabBarTabVSpace:
    case QStyle::PM_TabBarBaseHeight:
    case QStyle::PM_TabBarBaseOverlap:
    case QStyle::PM_ProgressBarChunkWidth:
    case QStyle::PM_SplitterWidth:
    case QStyle::PM_TitleBarHeight:
    case QStyle::PM_MenuScrollerHeight:
    case QStyle::PM_MenuHMargin:
    case QStyle::PM_MenuVMargin:
    case QStyle::PM_MenuPanelWidth:
    case QStyle::PM_MenuTearoffHeight:
    case QStyle::PM_MenuDesktopFrameWidth:
    case QStyle::PM_MenuBarPanelWidth:
    case QStyle::PM_MenuBarItemSpacing:
    case QStyle::PM_MenuBarVMargin:
    case QStyle::PM_MenuBarHMargin:
    case QStyle::PM_IndicatorWidth:
    case QStyle::PM_IndicatorHeight:
    case QStyle::PM_ExclusiveIndicatorWidth:
    case QStyle::PM_ExclusiveIndicatorHeight:
    case QStyle::PM_DialogButtonsSeparator:
    case QStyle::PM_DialogButtonsButtonWidth:
    case QStyle::PM_DialogButtonsButtonHeight:
    case QStyle::PM_MdiSubWindowFrameWidth:
    case QStyle::PM_MdiSubWindowMinimizedWidth:
    case QStyle::PM_HeaderMargin:
    case QStyle::PM_HeaderMarkSize:
    case QStyle::PM_HeaderGripMargin:
    case QStyle::PM_TabBarTabShiftHorizontal:
    case QStyle::PM_TabBarTabShiftVertical:
    case QStyle::PM_TabBarScrollButtonWidth:
    case QStyle::PM_ToolBarFrameWidth:
    case QStyle::PM_ToolBarHandleExtent:
    case QStyle::PM_ToolBarItemSpacing:
    case QStyle::PM_ToolBarItemMargin:
    case QStyle::PM_ToolBarSeparatorExtent:
    case QStyle::PM_ToolBarExtensionExtent:
    case QStyle::PM_SpinBoxSliderHeight:
    case QStyle::PM_ToolBarIconSize:
    case QStyle::PM_ListViewIconSize:
    case QStyle::PM_IconViewIconSize:
    case QStyle::PM_SmallIconSize:
    case QStyle::PM_LargeIconSize:
    case QStyle::PM_FocusFrameVMargin:
    case QStyle::PM_FocusFrameHMargin:
    case QStyle::PM_ToolTipLabelFrameWidth:
    case QStyle::PM_CheckBoxLabelSpacing:
    case QStyle::PM_TabBarIconSize:
    case QStyle::PM_SizeGripSize:
    case QStyle::PM_DockWidgetTitleMargin:
    case QStyle::PM_MessageBoxIconSize:
    case QStyle::PM_ButtonIconSize:
    case QStyle::PM_DockWidgetTitleBarButtonMargin:
    case QStyle::PM_RadioButtonLabelSpacing:
    case QStyle::PM_LayoutLeftMargin:
    case QStyle::PM_LayoutTopMargin:
    case QStyle::PM_LayoutRightMargin:
    case QStyle::PM_LayoutBottomMargin:
    case QStyle::PM_LayoutHorizontalSpacing:
    case QStyle::PM_LayoutVerticalSpacing:
    case QStyle::PM_TabBar_ScrollButtonOverlap:
    case QStyle::PM_TextCursorWidth:
    case QStyle::PM_TabCloseIndicatorWidth:
    case QStyle::PM_TabCloseIndicatorHeight:
    case QStyle::PM_ScrollView_ScrollBarSpacing:
    case QStyle::PM_ScrollView_ScrollBarOverlap:
    case QStyle::PM_SubMenuOverlap:
    case QStyle::PM_TreeViewIndentation:
    case QStyle::PM_HeaderDefaultSectionSizeHorizontal:
    case QStyle::PM_HeaderDefaultSectionSizeVertical:
    case QStyle::PM_TitleBarButtonIconSize:
    case QStyle::PM_TitleBarButtonSize:
    case QStyle::PM_LineEditIconSize:
    case QStyle::PM_LineEditIconMargin:
    case QStyle::PM_CustomBase:
        return QProxyStyle::pixelMetric(metric, option, widget);
        break;
    default:
        return QProxyStyle::pixelMetric(metric, option, widget);
    };
}

void UnionStyle::polish(QApplication *application)
{
    QProxyStyle::polish(application);

    // Set global window color
    auto element = Union::Element::create();
    element->setType(QStringLiteral("ApplicationWindow"));

    const auto style = Union::StyleRegistry::instance()->defaultStyle();
    const auto matches = style->matches({element});
    const auto properties = matches.first()->properties();

    QPalette palette;
    if (properties->background() && properties->background()->color()) {
        palette.setColor(QPalette::Window, properties->background()->color().value().toQColor());
    }

    application->setPalette(palette);
}

void UnionStyle::polish(QWidget *widget)
{
    if (qobject_cast<QPushButton *>(widget)) {
        widget->setAttribute(Qt::WA_Hover);
    }

    widget->setProperty(property_union_member_list, setupMemberList(widget));

    QProxyStyle::polish(widget);
}
