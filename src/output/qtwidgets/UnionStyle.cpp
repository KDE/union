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
    : QCommonStyle()
{
    Union::StyleRegistry::instance()->load();
}

void UnionStyle::drawControl(QStyle::ControlElement controlElement, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // Make lines not look completely terrible on fractional scales
    painter->setRenderHint(QPainter::Antialiasing, true);

    switch (controlElement) {
    case QStyle::CE_PushButtonBevel: {
        const auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option);
        auto opt = *buttonOption;
        opt.rect = subElementRect(SE_PushButtonBevel, buttonOption, widget);
        drawElement(queryProperties(prepareElements(buttonOption, widget)), painter, &opt);
    }
        return;
    case QStyle::CE_RadioButtonLabel:
    case QStyle::CE_CheckBoxLabel:
    case QStyle::CE_PushButtonLabel: {
        const auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option);
        drawIconText(buttonOption, this, painter, widget, buttonOption->icon, buttonOption->text);
    }
        return;
    case QStyle::CE_PushButton: {
        drawControl(CE_PushButtonBevel, option, painter, widget);
        drawControl(CE_PushButtonLabel, option, painter, widget);
    }
        return;
    case QStyle::CE_ToolButtonLabel: {
        const auto buttonOption = qstyleoption_cast<const QStyleOptionToolButton *>(option);
        auto text = buttonOption->text;
        auto icon = buttonOption->icon;
        // Skip text drawing for icon only buttons completely
        if (buttonOption->toolButtonStyle == Qt::ToolButtonIconOnly) {
            text = QString();
        }
        if (buttonOption->toolButtonStyle == Qt::ToolButtonTextOnly) {
            icon = QIcon();
        }
        drawIconText(buttonOption, this, painter, widget, icon, text);
    }
        return;
    case QStyle::CE_CheckBox: {
        const auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option);
        QStyleOptionButton subopt = *buttonOption;
        // Draw background
        auto bgElements = prepareElements(&subopt, widget, {QStringLiteral("CheckBox")});
        auto bgProps = queryProperties(bgElements);
        auto rect = backgroundRectangle(option, bgProps).toRect();
        drawBackground(painter, rect, bgProps);
        // Draw indicator
        subopt.rect = subElementRect(SE_CheckBoxIndicator, buttonOption, widget);
        drawPrimitive(PE_IndicatorCheckBox, &subopt, painter, widget);
        // Draw text
        subopt.rect = subElementRect(SE_CheckBoxContents, buttonOption, widget);
        drawControl(CE_CheckBoxLabel, &subopt, painter, widget);
    }
        return;
    case QStyle::CE_RadioButton: {
        const auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option);
        QStyleOptionButton subopt = *buttonOption;
        // Draw background
        auto bgElements = prepareElements(&subopt, widget, {QStringLiteral("RadioButton")});
        auto bgProps = queryProperties(bgElements);
        auto rect = backgroundRectangle(option, bgProps).toRect();
        drawBackground(painter, rect, bgProps);
        // Draw indicator
        subopt.rect = subElementRect(SE_RadioButtonIndicator, buttonOption, widget);
        drawPrimitive(PE_IndicatorRadioButton, &subopt, painter, widget);
        // Draw text
        subopt.rect = subElementRect(SE_RadioButtonContents, buttonOption, widget);
        drawControl(CE_RadioButtonLabel, &subopt, painter, widget);
    }
        return;
    case QStyle::CE_MenuItem: {
        const auto menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option);
        drawIconText(option, this, painter, widget, menuItem->icon, menuItem->text);
    }
        return;
    case QStyle::CE_TabBarTabShape: {
        const auto tabOption = qstyleoption_cast<const QStyleOptionTab *>(option);
        auto opt = *tabOption;
        opt.rect = subElementRect(SE_TabWidgetTabPane, tabOption, widget);
        drawElement(queryProperties(prepareElements(tabOption, widget)), painter, tabOption);
    }
        return;
    case QStyle::CE_TabBarTabLabel: {
        const auto tabOption = qstyleoption_cast<const QStyleOptionTab *>(option);
        auto opt = *tabOption;
        opt.rect = subElementRect(SE_TabBarTabText, tabOption, widget);
        drawIconText(&opt, this, painter, widget, tabOption->icon, tabOption->text);
    }
        return;
    case QStyle::CE_TabBarTab: {
        drawControl(CE_TabBarTabShape, option, painter, widget);
        drawControl(CE_TabBarTabLabel, option, painter, widget);
    }
        return;
    case QStyle::CE_ProgressBar:
    case QStyle::CE_ProgressBarGroove:
    case QStyle::CE_ProgressBarContents:
    case QStyle::CE_ProgressBarLabel:
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

    QCommonStyle::drawControl(controlElement, option, painter, widget);
}

// Complex controls are bit annoying. We may need to manually handle some things to make sure they work correctly
void UnionStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    switch (control) {
    case QStyle::CC_ToolButton: {
        const auto buttonOption = qstyleoption_cast<const QStyleOptionToolButton *>(option);
        drawElement(queryProperties(prepareElements(option, widget)), painter, buttonOption);
        drawControl(CE_ToolButtonLabel, buttonOption, painter, widget);
    }
        return;
    case QStyle::CC_GroupBox: {
        const auto groupBoxOption = qstyleoption_cast<const QStyleOptionGroupBox *>(option);
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

    QCommonStyle::drawComplexControl(control, option, painter, widget);
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

    QCommonStyle::drawPrimitive(element, option, painter, widget);
}

QSize UnionStyle::sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *widget) const
{
    // TODO use subelement rects to build the thing if possible
    QSize size = QCommonStyle::sizeFromContents(ct, opt, contentsSize, widget);
    QSize minimumSize(contentsSize.width(), contentsSize.height());
    QIcon icon = QIcon();
    QString text = QString();
    QStringList subElements = {};
    bool vertical = false;

    switch (ct) {
    case QStyle::CT_CheckBox: {
        QRegion elements;
        auto indicatorRect = subElementRect(SE_CheckBoxIndicator, opt, widget);
        auto textRect = subElementRect(SE_CheckBoxContents, opt, widget);
        elements.setRects({indicatorRect, textRect});
        return elements.boundingRect().size();
    }
    case QStyle::CT_RadioButton: {
        QRegion elements;
        auto indicatorRect = subElementRect(SE_RadioButtonIndicator, opt, widget);
        auto textRect = subElementRect(SE_RadioButtonContents, opt, widget);
        elements.setRects({indicatorRect, textRect});
        return elements.boundingRect().size();
    }
    case QStyle::CT_PushButton: {
        const auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(opt);
        icon = buttonOption->icon;
        text = buttonOption->text;
    } break;
    case QStyle::CT_ToolButton: {
        const auto toolButtonOption = qstyleoption_cast<const QStyleOptionToolButton *>(opt);
        icon = toolButtonOption->icon;
        text = toolButtonOption->text;
        vertical = toolButtonOption->toolButtonStyle == Qt::ToolButtonStyle::ToolButtonTextUnderIcon;
    } break;
    case QStyle::CT_ComboBox: {
        const auto comboBoxOption = qstyleoption_cast<const QStyleOptionComboBox *>(opt);
        icon = comboBoxOption->currentIcon;
        text = comboBoxOption->currentText;
    } break;
    case QStyle::CT_TabBarTab: {
        const auto tabOption = qstyleoption_cast<const QStyleOptionTab *>(opt);
        icon = tabOption->icon;
        text = tabOption->text;
    } break;
    case QStyle::CT_MenuBar:
    case QStyle::CT_MenuItem:
    case QStyle::CT_MenuBarItem: {
        const auto menuOption = qstyleoption_cast<const QStyleOptionMenuItem *>(opt);
        icon = menuOption->icon;
        text = menuOption->text;
    } break;
    case QStyle::CT_GroupBox: {
        const auto groupBoxOption = qstyleoption_cast<const QStyleOptionGroupBox *>(opt);
        text = groupBoxOption->text;
    } break;
    case QStyle::CT_ProgressBar: {
        const auto progressBarOption = qstyleoption_cast<const QStyleOptionProgressBar *>(opt);
        text = progressBarOption->text;
    } break;
    case QStyle::CT_HeaderSection: {
        const auto headerOption = qstyleoption_cast<const QStyleOptionHeader *>(opt);
        icon = headerOption->icon;
        text = headerOption->text;
    } break;
    case QStyle::CT_ItemViewItem: {
        const auto viewItemOption = qstyleoption_cast<const QStyleOptionViewItem *>(opt);
        icon = viewItemOption->icon;
        text = viewItemOption->text;
    } break;
    // QStyleOptionSlider, no text/icon
    case QStyle::CT_Slider:
    case QStyle::CT_ScrollBar:
    // QStyleOptionFrame, no text/icon
    case QStyle::CT_LineEdit:
    // QStyleOptionSpinBox, no text/icon
    case QStyle::CT_SpinBox:
    // QStyleOptionTabWidgetFrame, no text/icon
    case QStyle::CT_TabWidget:
    // Undocumented
    case QStyle::CT_DialogButtons:
    // QStyleOptionComplex, no text/icon
    case QStyle::CT_MdiControls:
    // Uses QStyleOption so there is no text/icon information
    case QStyle::CT_SizeGrip:
    case QStyle::CT_Menu:
    case QStyle::CT_Splitter:
    case QStyle::CT_CustomBase:
        break;
    }

    auto elements = prepareElements(opt, widget, subElements);
    if (elements.isEmpty()) {
        return size;
    }
    auto properties = queryProperties(elements);
    if (!properties) {
        return size;
    }
    QMargins padding;
    if (properties->layout()) {
        auto width = properties->layout()->width().value_or(contentsSize.width());
        auto height = properties->layout()->height().value_or(contentsSize.height());
        minimumSize = QSize(width, height);
        if (properties->layout()->padding()) {
            padding = properties->layout()->padding()->toMargins().toMargins();
        }
    }

    if (!text.isEmpty()) {
        const int textFlags(Qt::TextShowMnemonic | Qt::AlignCenter);
        size = opt->fontMetrics.size(textFlags, text);
    }
    if (!icon.isNull() && properties->icon()) {
        QSize iconSize(properties->icon()->width().value_or(0), properties->icon()->height().value_or(0));
        if (vertical) {
            size.setHeight(size.height() + iconSize.height() + padding.top() + padding.bottom());
        } else {
            size.setHeight(qMax(size.height(), iconSize.height()) + padding.top() + padding.bottom());
        }
        size.rwidth() += iconSize.width() + padding.right() + padding.left();

        if (!text.isEmpty() && properties->layout()->spacing()) {
            size.rwidth() += properties->layout()->spacing().value_or(0);
        }
    } else {
        size = size.grownBy(padding);
    }

    if (size.width() < minimumSize.width()) {
        size.setWidth(minimumSize.width());
    }
    if (size.height() < minimumSize.height()) {
        size.setHeight(minimumSize.height());
    }
    return size;
}

QRect UnionStyle::subElementRect(QStyle::SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    switch (element) {
    case QStyle::SE_CheckBoxIndicator: {
        auto checkboxElements = prepareElements(option, widget, {QStringLiteral("CheckBox")});
        auto indicatorMap = layoutMap(checkboxElements, option, {QStringLiteral("Indicator")});
        return indicatorMap[QStringLiteral("Indicator")].rect.toRect();
    }
    case QStyle::SE_CheckBoxContents: {
        auto checkboxElements = prepareElements(option, widget, {QStringLiteral("CheckBox")});
        auto indicatorMap = layoutMap(checkboxElements, option, {QStringLiteral("Indicator"), QStringLiteral("Text")});
        return indicatorMap[QStringLiteral("Text")].rect.toRect();
    }
    case QStyle::SE_RadioButtonIndicator: {
        auto checkboxElements = prepareElements(option, widget, {QStringLiteral("RadioButton")});
        auto indicatorMap = layoutMap(checkboxElements, option, {QStringLiteral("Indicator")});
        return indicatorMap[QStringLiteral("Indicator")].rect.toRect();
    }
    case QStyle::SE_RadioButtonContents: {
        auto checkboxElements = prepareElements(option, widget, {QStringLiteral("RadioButton")});
        auto indicatorMap = layoutMap(checkboxElements, option, {QStringLiteral("Indicator"), QStringLiteral("Text")});
        return indicatorMap[QStringLiteral("Text")].rect.toRect();
    }
    case QStyle::SE_DockWidgetCloseButton:
    case QStyle::SE_DockWidgetFloatButton:
    case QStyle::SE_DockWidgetIcon:
    case QStyle::SE_DockWidgetTitleBarText:
    case QStyle::SE_FrameContents:
    case QStyle::SE_HeaderArrow:
    case QStyle::SE_HeaderLabel:
    case QStyle::SE_ItemViewItemCheckIndicator:
    case QStyle::SE_ItemViewItemDecoration:
    case QStyle::SE_ItemViewItemText:
    case QStyle::SE_LineEditContents:
    case QStyle::SE_ProgressBarContents:
    case QStyle::SE_ProgressBarGroove:
    case QStyle::SE_ProgressBarLabel:
    case QStyle::SE_PushButtonBevel:
    case QStyle::SE_PushButtonContents:
    case QStyle::SE_ShapedFrameContents:
    case QStyle::SE_TabBarScrollLeftButton:
    case QStyle::SE_TabBarScrollRightButton:
    case QStyle::SE_TabBarTabLeftButton:
    case QStyle::SE_TabBarTabRightButton:
    case QStyle::SE_TabBarTabText:
    case QStyle::SE_TabBarTearIndicator:
    case QStyle::SE_TabBarTearIndicatorRight:
    case QStyle::SE_TabWidgetLeftCorner:
    case QStyle::SE_TabWidgetRightCorner:
    case QStyle::SE_TabWidgetTabBar:
    case QStyle::SE_TabWidgetTabContents:
    case QStyle::SE_TabWidgetTabPane:
    case QStyle::SE_ToolBarHandle:
    case QStyle::SE_ToolBoxTabContents:
    case QStyle::SE_TreeViewDisclosureItem:
        break;
    // Follow defaults
    case QStyle::SE_CustomBase:
    case QStyle::SE_CheckBoxClickRect:
    case QStyle::SE_CheckBoxFocusRect:
    case QStyle::SE_RadioButtonFocusRect:
    case QStyle::SE_RadioButtonClickRect:
    case QStyle::SE_PushButtonFocusRect:
    case QStyle::SE_ComboBoxFocusRect:
    case QStyle::SE_SliderFocusRect:
    case QStyle::SE_ItemViewItemFocusRect:
    case QStyle::SE_CheckBoxLayoutItem:
    case QStyle::SE_ComboBoxLayoutItem:
    case QStyle::SE_DateTimeEditLayoutItem:
    case QStyle::SE_FrameLayoutItem:
    case QStyle::SE_GroupBoxLayoutItem:
    case QStyle::SE_LabelLayoutItem:
    case QStyle::SE_SpinBoxLayoutItem:
    case QStyle::SE_SliderLayoutItem:
    case QStyle::SE_ProgressBarLayoutItem:
    case QStyle::SE_PushButtonLayoutItem:
    case QStyle::SE_RadioButtonLayoutItem:
    case QStyle::SE_TabWidgetLayoutItem:
    case QStyle::SE_ToolButtonLayoutItem:
        return QCommonStyle::subElementRect(element, option, widget);
    }

    return QCommonStyle::subElementRect(element, option, widget);
}

int UnionStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    int defaultMetric = QCommonStyle::pixelMetric(metric, option, widget);
    auto elements = prepareElements(option, widget);
    if (elements.isEmpty()) {
        return defaultMetric;
    }
    auto properties = queryProperties(elements);
    if (!properties) {
        return defaultMetric;
    }

    switch (metric) {
    // Don't shift button text when sunken
    case QStyle::PM_TabBarTabShiftHorizontal:
    case QStyle::PM_TabBarTabShiftVertical:
    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
        return 0;
    // Due to how QWidgets works, we just return the average margin size now
    // since there is no support for returning margin per edge
    case QStyle::PM_ToolBarItemMargin:
    case QStyle::PM_MenuBarVMargin:
    case QStyle::PM_MenuBarHMargin:
    case QStyle::PM_FocusFrameVMargin:
    case QStyle::PM_FocusFrameHMargin:
    case QStyle::PM_MenuHMargin:
    case QStyle::PM_MenuVMargin:
    case QStyle::PM_HeaderMargin:
    case QStyle::PM_LineEditIconMargin:
    case QStyle::PM_ButtonMargin: {
        if (properties->layout() && properties->layout()->margins()) {
            auto margins = properties->layout()->margins()->toMargins();
            auto avg = (margins.left() + margins.right() + margins.top() + margins.bottom()) / 4;
            return avg;
        }
    } break;
    case QStyle::PM_ToolBarSeparatorExtent:
    case QStyle::PM_ToolTipLabelFrameWidth:
    case QStyle::PM_ToolBarFrameWidth:
    case QStyle::PM_MenuDesktopFrameWidth:
    case QStyle::PM_MenuBarPanelWidth:
    case QStyle::PM_DefaultFrameWidth:
    case QStyle::PM_SpinBoxFrameWidth:
    case QStyle::PM_ComboBoxFrameWidth:
    case QStyle::PM_DockWidgetFrameWidth:
    case QStyle::PM_MdiSubWindowFrameWidth:
    case QStyle::PM_ButtonDefaultIndicator: {
        if (properties->border()) {
            auto borderSizes = properties->border()->sizes();
            auto avg = (borderSizes.left() + borderSizes.right() + borderSizes.top() + borderSizes.bottom()) / 4;
            return avg;
        }
    } break;
    case QStyle::PM_IndicatorWidth:
    case QStyle::PM_ExclusiveIndicatorWidth:
    case QStyle::PM_MenuButtonIndicator:
    case QStyle::PM_IndicatorHeight:
    case QStyle::PM_TabCloseIndicatorWidth:
    case QStyle::PM_TabCloseIndicatorHeight:
    case QStyle::PM_ExclusiveIndicatorHeight: {
        auto elements = prepareElements(option, widget, {QStringLiteral("Indicator")});
        if (elements.isEmpty()) {
            return defaultMetric;
        }
        auto properties = queryProperties(elements);
        if (!properties) {
            return defaultMetric;
        }
        if (properties->layout()) {
            if (metric == PM_IndicatorHeight || metric == PM_ExclusiveIndicatorHeight || metric == PM_TabCloseIndicatorHeight) {
                return properties->layout()->height().value_or(defaultMetric);
            }
            return properties->layout()->width().value_or(defaultMetric);
        }
    } break;
    case QStyle::PM_ScrollBarSliderMin:
    case QStyle::PM_SliderThickness:
    case QStyle::PM_SliderLength:
    case QStyle::PM_ScrollBarExtent: {
        auto scrollbaroption = qstyleoption_cast<const QStyleOptionSlider *>(option);
        if (scrollbaroption && properties->layout()) {
            if (scrollbaroption->orientation == Qt::Horizontal) {
                return properties->layout()->height().value_or(defaultMetric);
            } else {
                return properties->layout()->width().value_or(defaultMetric);
            }
        }
    } break;
    case QStyle::PM_SliderControlThickness: {
        auto scrollbaroption = qstyleoption_cast<const QStyleOptionSlider *>(option);
        auto elements = prepareElements(option, widget, {QStringLiteral("Handle")});
        if (elements.isEmpty()) {
            return defaultMetric;
        }
        auto properties = queryProperties(elements);
        if (!properties) {
            return defaultMetric;
        }
        if (scrollbaroption && properties->layout()) {
            if (scrollbaroption->orientation == Qt::Horizontal) {
                return properties->layout()->height().value_or(defaultMetric);
            } else {
                return properties->layout()->width().value_or(defaultMetric);
            }
        }
    } break;
    case QStyle::PM_ToolBarHandleExtent:
    case QStyle::PM_ToolBarExtensionExtent: {
        auto toolbaroption = qstyleoption_cast<const QStyleOptionToolBar *>(option);
        QStringList childelements;
        if (metric == PM_ToolBarHandleExtent) {
            childelements.append(QStringLiteral("Handle"));
        }
        if (metric == PM_ToolBarExtensionExtent) {
            childelements.append(QStringLiteral("Extension"));
        }
        auto elements = prepareElements(option, widget, childelements);
        if (elements.isEmpty()) {
            return defaultMetric;
        }
        auto properties = queryProperties(elements);
        if (!properties) {
            return defaultMetric;
        }
        if (toolbaroption && properties->layout()) {
            if (toolbaroption->state & QStyle::State_Horizontal) {
                return properties->layout()->width().value_or(defaultMetric);
            } else {
                return properties->layout()->height().value_or(defaultMetric);
            }
        }
    } break;
    case QStyle::PM_LayoutLeftMargin:
    case QStyle::PM_LayoutTopMargin:
    case QStyle::PM_LayoutRightMargin:
    case QStyle::PM_LayoutBottomMargin: {
        if (properties->layout() && properties->layout()->margins()) {
            auto margins = properties->layout()->margins()->toMargins();
            if (metric == PM_LayoutLeftMargin) {
                return margins.left();
            }
            if (metric == PM_LayoutTopMargin) {
                return margins.top();
            }
            if (metric == PM_LayoutBottomMargin) {
                return margins.bottom();
            }
            if (metric == PM_LayoutRightMargin) {
                return margins.right();
            }
        }
    } break;
    // Currently we only have one spacing value
    case QStyle::PM_CheckBoxLabelSpacing:
    case QStyle::PM_ScrollView_ScrollBarSpacing:
    case QStyle::PM_RadioButtonLabelSpacing:
    case QStyle::PM_TabBarTabHSpace:
    case QStyle::PM_TabBarTabVSpace:
    case QStyle::PM_MenuBarItemSpacing:
    case QStyle::PM_ToolBarItemSpacing:
    case QStyle::PM_LayoutHorizontalSpacing:
    case QStyle::PM_LayoutVerticalSpacing: {
        if (properties->layout()) {
            return properties->layout()->spacing().value_or(defaultMetric);
        }
    } break;
    case QStyle::PM_TitleBarButtonSize:
    case QStyle::PM_TabBarScrollButtonWidth:
    case QStyle::PM_MenuPanelWidth:
    case QStyle::PM_SplitterWidth:
    case QStyle::PM_ProgressBarChunkWidth: {
        if (properties->layout()) {
            return properties->layout()->width().value_or(defaultMetric);
        }
    } break;
    case QStyle::PM_SpinBoxSliderHeight:
    case QStyle::PM_TitleBarHeight:
    case QStyle::PM_MenuScrollerHeight:
    case QStyle::PM_TabBarBaseHeight: {
        if (properties->layout()) {
            return properties->layout()->height().value_or(defaultMetric);
        }
    } break;
    case QStyle::PM_TreeViewIndentation: {
        auto elements = prepareElements(option, widget, {QStringLiteral("Indentation")});
        if (elements.isEmpty()) {
            return defaultMetric;
        }
        auto properties = queryProperties(elements);
        if (!properties) {
            return defaultMetric;
        }
        if (properties->layout()) {
            return properties->layout()->width().value_or(defaultMetric);
        }
    } break;

    case QStyle::PM_ToolBarIconSize:
    case QStyle::PM_ListViewIconSize:
    case QStyle::PM_IconViewIconSize:
    case QStyle::PM_SmallIconSize:
    case QStyle::PM_LargeIconSize:
    case QStyle::PM_TabBarIconSize:
    case QStyle::PM_ButtonIconSize:
    case QStyle::PM_MessageBoxIconSize:
    case QStyle::PM_TitleBarButtonIconSize:
    case QStyle::PM_LineEditIconSize: {
        if (properties->icon()) {
            return properties->icon()->width().value_or(defaultMetric);
        }
    } break;

    // Use defaults
    case QStyle::PM_MaximumDragDistance:
    case QStyle::PM_SliderTickmarkOffset:
    case QStyle::PM_SliderSpaceAvailable:
    case QStyle::PM_MenuTearoffHeight:
    case QStyle::PM_DockWidgetSeparatorExtent:
    case QStyle::PM_TabBarTabOverlap:
    case QStyle::PM_DockWidgetHandleExtent:
    case QStyle::PM_TabBarBaseOverlap:
    case QStyle::PM_DialogButtonsSeparator:
    case QStyle::PM_DialogButtonsButtonWidth:
    case QStyle::PM_DialogButtonsButtonHeight:
    case QStyle::PM_MdiSubWindowMinimizedWidth:
    case QStyle::PM_HeaderMarkSize:
    case QStyle::PM_HeaderGripMargin:
    case QStyle::PM_DockWidgetTitleMargin:
    case QStyle::PM_DockWidgetTitleBarButtonMargin:
    case QStyle::PM_TabBar_ScrollButtonOverlap:
    case QStyle::PM_SizeGripSize:
    case QStyle::PM_TextCursorWidth:
    case QStyle::PM_ScrollView_ScrollBarOverlap:
    case QStyle::PM_SubMenuOverlap:
    case QStyle::PM_HeaderDefaultSectionSizeHorizontal:
    case QStyle::PM_HeaderDefaultSectionSizeVertical:
    case QStyle::PM_CustomBase:
    default:
        return QCommonStyle::pixelMetric(metric, option, widget);
    };
    return QCommonStyle::pixelMetric(metric, option, widget);
}

void UnionStyle::polish(QApplication *application)
{
    QCommonStyle::polish(application);

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

    QCommonStyle::polish(widget);
}
