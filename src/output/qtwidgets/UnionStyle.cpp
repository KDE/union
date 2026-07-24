// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "UnionStyle.h"
#include "StyleDrawing.h"
#include "StyleUtils.h"

#include <ElementQuery.h>
#include <QApplication>
#include <StyleRegistry.h>

#include <QApplication>
#include <QBitmap>
#include <QCheckBox>
#include <QComboBox>
#include <QDial>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDockWidget>
#include <QFormLayout>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QGroupBox>
#include <QItemDelegate>
#include <QLineEdit>
#include <QMainWindow>
#include <QMdiArea>
#include <QMenu>
#include <QMenuBar>
#include <QMetaEnum>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSplitterHandle>
#include <QStackedLayout>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleOption>
#include <QTableView>
#include <QTextBrowser>
#include <QTextEdit>
#include <QToolBar>
#include <QToolBox>
#include <QToolButton>
#include <QTreeView>
#include <QWidget>
#include <QWidgetAction>

/*
 * How this works:
 * We polish the items which should get the hierarchy information
 * Create the needed areas/rectangles/sizes from the layoutmap
 * Utilize those areas for drawing the elements in right places
 */

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
    case QStyle::CE_CheckBoxLabel: {
        const auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option);
        layoutAndDrawIconText(buttonOption, painter, widget, buttonOption->icon, buttonOption->text);
    }
        return;
    case QStyle::CE_PushButtonLabel: {
        const auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option);
        layoutAndDrawIconText(buttonOption, painter, widget, buttonOption->icon, buttonOption->text);
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
        } else if (buttonOption->toolButtonStyle == Qt::ToolButtonTextOnly) {
            icon = QIcon();
        }
        if (icon.isNull() && buttonOption->toolButtonStyle != Qt::ToolButtonTextOnly && buttonOption->features.testFlag(QStyleOptionToolButton::Arrow)) {
            QList<Union::Element::Ptr> elements = prepareElements(buttonOption, widget);
            bool hasText = !text.isEmpty();
            QStringList subElements = {QStringLiteral("Icon")};
            if (hasText) {
                subElements.append(QStringLiteral("Text"));
            }
            auto map = layoutMap(elements, buttonOption, subElements);
            if (hasText) {
                QRect textRect = map[QStringLiteral("Text")].rect.toRect();
                drawText(textRect, buttonOption, painter, text, widget);
            }
            QRect iconRect = map[QStringLiteral("Icon")].rect.toRect();
            auto subopt = *buttonOption;
            subopt.rect = iconRect;

            switch (buttonOption->arrowType) {
            case Qt::LeftArrow:
                drawPrimitive(PE_IndicatorArrowLeft, &subopt, painter, widget);
                break;
            case Qt::RightArrow:
                drawPrimitive(PE_IndicatorArrowRight, &subopt, painter, widget);
                break;
            case Qt::UpArrow:
                drawPrimitive(PE_IndicatorArrowUp, &subopt, painter, widget);
                break;
            case Qt::DownArrow:
                drawPrimitive(PE_IndicatorArrowDown, &subopt, painter, widget);
                break;
            default:
                break;
            }
        } else {
            layoutAndDrawIconText(buttonOption, painter, widget, icon, text);
        }
    }
        return;
    case QStyle::CE_CheckBox: {
        const auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option);
        QStyleOptionButton subopt = *buttonOption;
        // Draw background
        auto bgElements = prepareElements(&subopt, widget);
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
        auto bgElements = prepareElements(&subopt, widget);
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
        drawElement(queryProperties(prepareElements(menuItem, widget, {QStringLiteral("MenuItem")})), painter, menuItem);
        layoutAndDrawIconText(menuItem, painter, widget, menuItem->icon, menuItem->text);
    }
        return;
    case QStyle::CE_ToolBoxTabShape:
    case QStyle::CE_TabBarTabShape: {
        const auto tabOption = qstyleoption_cast<const QStyleOptionTab *>(option);
        auto bgElements = prepareElements(tabOption, widget, {QStringLiteral("TabButton")});
        auto bgProps = queryProperties(bgElements);
        auto rect = backgroundRectangle(option, bgProps).toRect();
        drawBackground(painter, rect, bgProps);
    }
        return;
    case QStyle::CE_ToolBoxTabLabel:
    case QStyle::CE_TabBarTabLabel: {
        const auto tabOption = qstyleoption_cast<const QStyleOptionTab *>(option);
        auto subopt = *tabOption;
        subopt.rect = subElementRect(SE_TabBarTabText, tabOption, widget);
        layoutAndDrawIconText(&subopt, painter, widget, tabOption->icon, tabOption->text);
    }
        return;
    case QStyle::CE_ToolBoxTab:
    case QStyle::CE_TabBarTab: {
        const auto tabOption = qstyleoption_cast<const QStyleOptionTab *>(option);
        drawControl(CE_TabBarTabShape, tabOption, painter, widget);
        drawControl(CE_TabBarTabLabel, tabOption, painter, widget);
    }
        return;
    case QStyle::CE_ItemViewItem: {
        const auto vi = qstyleoption_cast<const QStyleOptionViewItem *>(option);
        // Dont allow drawing outside of the area
        painter->save();
        painter->setClipRect(option->rect);
        QStyleOptionViewItem subopt = *vi;
        // Draw background
        auto elements = prepareElements(&subopt, widget, {QStringLiteral("ItemViewItem")});
        auto props = queryProperties(elements);
        subopt.rect = backgroundRectangle(option, props).toRect();
        drawBackground(painter, subopt.rect, props);
        // Draw text
        if (subopt.features.testFlag(QStyleOptionViewItem::HasDisplay)) {
            subopt.rect = subElementRect(SE_ItemViewItemText, &subopt, widget);
            layoutAndDrawIconText(&subopt, painter, widget, QIcon(), vi->text);
        }
        // Draw indicator
        if (subopt.features.testFlag(QStyleOptionViewItem::HasCheckIndicator)) {
            QStyleOptionButton checkbox;
            switch (subopt.checkState) {
            case Qt::Unchecked:
                checkbox.state = State_Off;
                break;
            case Qt::PartiallyChecked:
                checkbox.state = State_NoChange;
                break;
            case Qt::Checked:
                checkbox.state = State_On;
                break;
            }
            checkbox.rect = subElementRect(SE_ItemViewItemCheckIndicator, vi, widget);
            drawPrimitive(PE_IndicatorCheckBox, &checkbox, painter, widget);
        } else if (subopt.features.testFlag(QStyleOptionViewItem::HasDecoration)) {
            subopt.rect = subElementRect(SE_ItemViewItemDecoration, vi, widget);
            layoutAndDrawIconText(&subopt, painter, widget, vi->icon, QString());
        }
        painter->restore();
    }
        return;
    case QStyle::CE_ProgressBarGroove: {
        const auto opt = qstyleoption_cast<const QStyleOptionProgressBar *>(option);
        auto subopt = *opt;
        auto groove = subElementRect(SE_ProgressBarGroove, opt, widget);
        subopt.rect = groove;
        drawElement(queryProperties(prepareElements(&subopt, widget)), painter, &subopt);
    }
        return;
    case QStyle::CE_ProgressBarContents: {
        const auto opt = qstyleoption_cast<const QStyleOptionProgressBar *>(option);
        auto subopt = *opt;

        const qreal p = opt->progress;
        if (p <= 0) {
            return;
        }
        const qreal min = opt->minimum;
        const qreal max = opt->maximum;
        const qreal percentage = (p - min) / (max - min);

        auto groove = subElementRect(SE_ProgressBarGroove, opt, widget);
        auto progress = subElementRect(SE_ProgressBarContents, opt, widget);

        const bool horizontal = opt->state.testFlag(QStyle::State_Horizontal);
        const bool inverted(opt->invertedAppearance);
        bool reverse = horizontal && option->direction == Qt::RightToLeft;
        if (inverted) {
            reverse = !reverse;
        }

        if (horizontal) {
            const qreal progressWidth = percentage * groove.width();
            if (reverse) {
                progress.setLeft(groove.right() - progressWidth);
            } else {
                progress.setWidth(progressWidth);
            }
        } else {
            const qreal progressHeight = percentage * groove.height();
            if (reverse) {
                progress.setHeight(progressHeight);
            } else {
                progress.setTop(groove.bottom() - progressHeight);
            }
        }
        subopt.rect = progress;

        drawElement(queryProperties(prepareElements(&subopt, widget, {QStringLiteral("Track")})), painter, &subopt);
    }
        return;
    case QStyle::CE_ProgressBarLabel: {
        const auto opt = qstyleoption_cast<const QStyleOptionProgressBar *>(option);
        if (opt->textVisible) {
            auto subopt = *opt;
            auto rect = subElementRect(SE_ProgressBarLabel, opt, widget);
            subopt.rect = rect;
            layoutAndDrawIconText(&subopt, painter, widget, QIcon(), opt->text);
        }
    }
        return;
    case QStyle::CE_ProgressBar:
        drawControl(CE_ProgressBarGroove, option, painter, widget);
        drawControl(CE_ProgressBarContents, option, painter, widget);
        drawControl(CE_ProgressBarLabel, option, painter, widget);
        return;
    case QStyle::CE_MenuEmptyArea:
        return; // This is what breeze does as well
    case QStyle::CE_MenuBarItem:
    case QStyle::CE_MenuBarEmptyArea:
    case QStyle::CE_MenuVMargin:
    case QStyle::CE_MenuHMargin:
    case QStyle::CE_MenuTearoff:
    case QStyle::CE_MenuScroller:
    case QStyle::CE_Header:
    case QStyle::CE_HeaderSection:
    case QStyle::CE_HeaderLabel:
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
    case QStyle::CE_HeaderEmptyArea:
    case QStyle::CE_ColumnViewGrip:
    case QStyle::CE_ShapedFrame:
    case QStyle::CE_CustomBase:
        break;
    }

    QCommonStyle::drawControl(controlElement, option, painter, widget);
}

// Complex controls are bit annoying. We may need to manually handle some things to make sure they work correctly
void UnionStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    // Make lines not look completely terrible on fractional scales
    painter->setRenderHint(QPainter::Antialiasing, true);

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
                         textFlagsFromProperties(properties, true),
                         groupBoxOption->palette,
                         groupBoxOption->state & State_Enabled,
                         groupBoxOption->text,
                         textColor.isValid() ? QPalette::NoRole : QPalette::WindowText);
        }
    }
        return;
    case QStyle::CC_ComboBox: {
        // TODO: this is wrong, used for debug for now
        const auto buttonOption = qstyleoption_cast<const QStyleOptionComboBox *>(option);
        const auto elements = prepareElements(buttonOption, widget);
        const auto properties = queryProperties(elements);
        auto rect = backgroundRectangle(buttonOption, properties).toRect();
        drawBackground(painter, rect, properties);
        drawControl(CE_ComboBoxLabel, buttonOption, painter, widget);
    }
        return;
    case QStyle::CC_SpinBox:
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
    // Make lines not look completely terrible on fractional scales
    painter->setRenderHint(QPainter::Antialiasing, true);

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
    case QStyle::PE_IndicatorArrowLeft: {
        auto props = queryProperties(prepareElements(option, widget, {QStringLiteral("IndicatorArrowLeft")}));
        auto name = QStringLiteral("arrow-left-symbolic");
        if (props->icon()) {
            name = props->icon()->name().value_or(name);
        }
        auto icon = QIcon::fromTheme(name);
        drawIcon(option->rect, option, painter, icon, widget);
        return;
    }
    case QStyle::PE_IndicatorArrowUp: {
        auto props = queryProperties(prepareElements(option, widget, {QStringLiteral("IndicatorArrowUp")}));
        auto name = QStringLiteral("arrow-up-symbolic");
        if (props->icon()) {
            name = props->icon()->name().value_or(name);
        }
        auto icon = QIcon::fromTheme(name);
        drawIcon(option->rect, option, painter, icon, widget);
        return;
    }
    case QStyle::PE_IndicatorArrowRight: {
        auto props = queryProperties(prepareElements(option, widget, {QStringLiteral("IndicatorArrowRight")}));
        auto name = QStringLiteral("arrow-right-symbolic");
        if (props->icon()) {
            name = props->icon()->name().value_or(name);
        }
        auto icon = QIcon::fromTheme(name);
        drawIcon(option->rect, option, painter, icon, widget);
        return;
    }
    case QStyle::PE_IndicatorArrowDown: {
        auto props = queryProperties(prepareElements(option, widget, {QStringLiteral("IndicatorArrowDown")}));
        auto name = QStringLiteral("arrow-down-symbolic");
        if (props->icon()) {
            name = props->icon()->name().value_or(name);
        }
        auto icon = QIcon::fromTheme(name);
        drawIcon(option->rect, option, painter, icon, widget);
        return;
    }
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
    auto elements = prepareElements(opt, widget);
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
    switch (ct) {
    case QStyle::CT_CheckBox: {
        QRegion r;
        auto indicatorRect = subElementRect(SE_CheckBoxIndicator, opt, widget);
        auto textRect = subElementRect(SE_CheckBoxContents, opt, widget);
        r.setRects({indicatorRect, textRect});
        size = r.boundingRect().size().grownBy(padding);
    } break;
    case QStyle::CT_RadioButton: {
        QRegion r;
        auto indicatorRect = subElementRect(SE_RadioButtonIndicator, opt, widget);
        auto textRect = subElementRect(SE_RadioButtonContents, opt, widget);
        r.setRects({indicatorRect, textRect});
        size = r.boundingRect().size().grownBy(padding);
    } break;
    case QStyle::CT_PushButton: {
        auto contentsRect = subElementRect(SE_PushButtonContents, opt, widget);
        size = contentsRect.size().grownBy(padding);
    } break;
    case QStyle::CT_ToolButton: {
        const auto toolButtonOption = qstyleoption_cast<const QStyleOptionToolButton *>(opt);
        auto elements = prepareElements(toolButtonOption, widget);
        QStringList childelements = {};
        if (!toolButtonOption->icon.isNull()) {
            childelements.append(QStringLiteral("Icon"));
        }
        if (!toolButtonOption->text.isEmpty()) {
            childelements.append(QStringLiteral("Text"));
        }
        if (childelements.isEmpty()) {
            return size;
        }
        auto map = layoutMap(elements, toolButtonOption, childelements);
        QRect unifiedRect;
        for (const auto &m : map) {
            unifiedRect = unifiedRect.united(m.rect.toRect());
        }
        size = unifiedRect.size().grownBy(padding);
    } break;
    case QStyle::CT_ComboBox: {
        const auto option = qstyleoption_cast<const QStyleOptionComboBox *>(opt);
        auto elements = prepareElements(option, widget);
        QStringList childelements = {QStringLiteral("Icon"), QStringLiteral("Text"), QStringLiteral("Indicator")};
        auto map = layoutMap(elements, option, childelements);
        QRect unifiedRect;
        for (const auto &m : map) {
            unifiedRect = unifiedRect.united(m.rect.toRect());
        }
        size = unifiedRect.size().grownBy(padding);
    } break;
    case QStyle::CT_TabBarTab: {
        auto elements = prepareElements(opt, widget, {QStringLiteral("TabButton")});
        auto props = queryProperties(elements);
        auto textRect = subElementRect(SE_TabBarTabText, opt, widget);
        if (props->layout()) {
            if (props->layout()->width()) {
                textRect.setWidth(props->layout()->width().value_or(contentsSize.width()));
            }
            if (props->layout()->height()) {
                textRect.setHeight(props->layout()->height().value_or(contentsSize.height()));
            }
            if (props->layout()->padding()) {
                padding = props->layout()->padding()->toMargins().toMargins();
            }
        }
        return textRect.size().grownBy(padding);
    } break;
    case QStyle::CT_MenuBar: {
        const auto option = qstyleoption_cast<const QStyleOptionMenuItem *>(opt);
        size = backgroundRectangle(option, properties).size().toSize();
    } break;
    case QStyle::CT_MenuBarItem: {
        const auto menuopt = qstyleoption_cast<const QStyleOptionMenuItem *>(opt);
        auto elements = prepareElements(menuopt, widget, {QStringLiteral("MenuBarItem")});
        QStringList childelements = {};
        if (!menuopt->icon.isNull()) {
            childelements.append(QStringLiteral("Icon"));
        }
        if (!menuopt->text.isEmpty()) {
            childelements.append(QStringLiteral("Text"));
        }
        if (childelements.isEmpty()) {
            return size;
        }
        auto map = layoutMap(elements, menuopt, childelements);
        QRect unifiedRect;
        for (const auto &m : map) {
            unifiedRect = unifiedRect.united(m.rect.toRect());
        }
        size = unifiedRect.size().grownBy(padding);
    } break;
    case QStyle::CT_MenuItem:
    case QStyle::CT_GroupBox: {
    } break;
    case QStyle::CT_ProgressBar: {
        QRegion r;
        auto grooveRect = subElementRect(SE_ProgressBarGroove, opt, widget);
        auto contentRect = subElementRect(SE_ProgressBarContents, opt, widget);
        auto textRect = subElementRect(SE_ProgressBarLabel, opt, widget);
        r.setRects({grooveRect, contentRect, textRect});
        size = r.boundingRect().size().grownBy(padding);
    } break;
    case QStyle::CT_HeaderSection: {
    } break;
    case QStyle::CT_ItemViewItem: {
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
    QRect rect;
    switch (element) {
    case QStyle::SE_TreeViewDisclosureItem:
    case QStyle::SE_RadioButtonIndicator:
    case QStyle::SE_CheckBoxIndicator: {
        auto elements = prepareElements(option, widget);
        auto map = layoutMap(elements, option, {QStringLiteral("Indicator")});
        rect = map[QStringLiteral("Indicator")].rect.toRect();
    } break;
    case QStyle::SE_ItemViewItemText:
    case QStyle::SE_ItemViewItemDecoration:
    case QStyle::SE_ItemViewItemCheckIndicator: {
        const auto vi = qstyleoption_cast<const QStyleOptionViewItem *>(option);
        QStringList childelements = {};
        if (vi) {
            if (vi->features.testFlag(QStyleOptionViewItem::HasDisplay)) {
                childelements.append(QStringLiteral("Text"));
            }
            if (vi->features.testFlag(QStyleOptionViewItem::HasDecoration)) {
                childelements.append(QStringLiteral("Icon"));
            }
            if (vi->features.testFlag(QStyleOptionViewItem::HasCheckIndicator)) {
                childelements.append(QStringLiteral("CheckBox"));
            }
        }
        if (childelements.empty()) {
            return QRect();
        }

        auto elements = prepareElements(option, widget, {QStringLiteral("ItemViewItem")});
        auto map = layoutMap(elements, option, childelements);

        if (element == SE_ItemViewItemText) {
            rect = map[QStringLiteral("Text")].rect.toRect();
        }
        if (element == SE_ItemViewItemDecoration) {
            rect = map[QStringLiteral("Icon")].rect.toRect();
        }
        if (element == SE_ItemViewItemCheckIndicator) {
            rect = map[QStringLiteral("CheckBox")].rect.toRect();
        }
    } break;

    case QStyle::SE_PushButtonContents:
    case QStyle::SE_RadioButtonContents:
    case QStyle::SE_CheckBoxContents: {
        const auto opt = qstyleoption_cast<const QStyleOptionButton *>(option);
        auto elements = prepareElements(option, widget);
        QStringList childelements = {};
        if (element != SE_PushButtonContents) {
            childelements.append(QStringLiteral("Indicator"));
        }
        if (!opt->icon.isNull()) {
            childelements.append(QStringLiteral("Icon"));
        }
        if (!opt->text.isEmpty()) {
            childelements.append(QStringLiteral("Text"));
        }
        if (childelements.isEmpty()) {
            return QRect();
        }
        auto map = layoutMap(elements, option, childelements);
        QRect unifiedRect;
        for (const auto &m : map) {
            // Contents will skip the indicator
            if (m.elementName != QStringLiteral("Indicator")) {
                unifiedRect = unifiedRect.united(m.rect.toRect());
            }
        }
        rect = unifiedRect;
    } break;

    case QStyle::SE_PushButtonBevel:
    case QStyle::SE_ShapedFrameContents:
    case QStyle::SE_LineEditContents:
    case QStyle::SE_FrameContents: {
        auto frameElements = prepareElements(option, widget);
        auto props = queryProperties(frameElements);
        int frameWidth = pixelMetric(PM_DefaultFrameWidth, option, widget);
        rect = backgroundRectangle(option, props).toRect().adjusted(frameWidth, frameWidth, -frameWidth, -frameWidth);
    } break;
    case QStyle::SE_HeaderArrow:
    case QStyle::SE_HeaderLabel: {
        auto elements = prepareElements(option, widget, {QStringLiteral("HeaderViewDelegate")});
        QStringList childelements = {QStringLiteral("Text"), QStringLiteral("Icon")};
        auto map = layoutMap(elements, option, childelements);
        auto mapItem = (element == SE_HeaderLabel) ? QStringLiteral("Text") : QStringLiteral("Icon");
        rect = map[mapItem].rect.toRect();
    } break;
    case QStyle::SE_TabBarTabText: {
        auto elements = prepareElements(option, widget, {QStringLiteral("TabButton")});
        auto map = layoutMap(elements, option, {QStringLiteral("Icon"), QStringLiteral("Text")});
        QRect unifiedRect;
        for (const auto &m : map) {
            unifiedRect = unifiedRect.united(m.rect.toRect());
        }
        rect = unifiedRect;
    } break;
    case QStyle::SE_ProgressBarLabel:
    case QStyle::SE_ProgressBarContents:
    case QStyle::SE_ProgressBarGroove: {
        const auto opt(qstyleoption_cast<const QStyleOptionProgressBar *>(option));
        if (!opt) {
            return QRect();
        }
        auto props = queryProperties(prepareElements(option, widget));
        qreal width = 0;
        qreal height = 0;
        if (props->layout()) {
            width = props->layout()->width().value_or(width);
            height = props->layout()->height().value_or(height);
        }
        rect = option->rect;
        if (opt->state.testFlag(QStyle::State_Horizontal)) {
            rect = centerRect(rect, width, height);
        } else {
            rect = centerRect(rect, height, width);
        }
    } break;
    // Follow defaults
    case QStyle::SE_TabWidgetTabContents:
    case QStyle::SE_ToolBoxTabContents:
    case QStyle::SE_TabBarTabLeftButton:
    case QStyle::SE_TabBarTabRightButton:
    case QStyle::SE_DockWidgetCloseButton:
    case QStyle::SE_DockWidgetFloatButton:
    case QStyle::SE_DockWidgetIcon:
    case QStyle::SE_DockWidgetTitleBarText:
    case QStyle::SE_ToolBarHandle:
    case QStyle::SE_TabWidgetLeftCorner:
    case QStyle::SE_TabWidgetRightCorner:
    case QStyle::SE_TabWidgetTabBar:
    case QStyle::SE_TabWidgetTabPane:
    case QStyle::SE_TabBarTearIndicator:
    case QStyle::SE_TabBarTearIndicatorRight:
    case QStyle::SE_TabBarScrollRightButton:
    case QStyle::SE_TabBarScrollLeftButton:
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

    return visualRect(option->direction, option->rect, rect);
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
    case QStyle::PM_TabBarScrollButtonWidth: {
        auto elements = prepareElements(option, widget, {QStringLiteral("TabScrollButton")});
        if (elements.isEmpty()) {
            return defaultMetric;
        }
        auto properties = queryProperties(elements);
        if (!properties) {
            return defaultMetric;
        }
        return properties->layout()->width().value_or(defaultMetric);
    } break;
    case QStyle::PM_TitleBarButtonSize:
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
    case QStyle::PM_TabBar_ScrollButtonOverlap:
        return 0;
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
    case QStyle::PM_SizeGripSize:
    case QStyle::PM_TextCursorWidth:
    case QStyle::PM_ScrollView_ScrollBarOverlap:
    case QStyle::PM_SubMenuOverlap:
    case QStyle::PM_HeaderDefaultSectionSizeHorizontal:
    case QStyle::PM_HeaderDefaultSectionSizeVertical:
    case QStyle::PM_CustomBase:
    default:
        break;
    };
    return QCommonStyle::pixelMetric(metric, option, widget);
}

// Copied from Breeze.
// TODO: Make these adjustable!
int UnionStyle::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    switch (hint) {
    case SH_RubberBand_Mask: {
        if (auto mask = qstyleoption_cast<QStyleHintReturnMask *>(returnData)) {
            mask->region = option->rect;

            /*
             * need to check on widget before removing inner region
             * in order to still preserve rubberband in MainWindow and QGraphicsView
             * in QMainWindow because it looks better
             * in QGraphicsView because the painting fails completely otherwise
             */
            if (widget
                && (qobject_cast<const QAbstractItemView *>(widget->parent()) || qobject_cast<const QGraphicsView *>(widget->parent())
                    || qobject_cast<const QMainWindow *>(widget->parent()))) {
                return true;
            }

            // also check if widget's parent is some itemView viewport
            if (widget && widget->parent() && qobject_cast<const QAbstractItemView *>(widget->parent()->parent())
                && static_cast<const QAbstractItemView *>(widget->parent()->parent())->viewport() == widget->parent()) {
                return true;
            }

            // mask out center
            mask->region -= option->rect.adjusted(1, 1, -1, -1);

            return true;
        }
        return false;
    }
    case SH_ComboBox_ListMouseTracking:
        return true;
    case SH_MenuBar_MouseTracking:
        return true;
    case SH_Menu_Scrollable:
        return true;
    case SH_Menu_MouseTracking:
        return true;
    case SH_Menu_SubMenuPopupDelay:
        return 150;
    case SH_Menu_SloppySubMenus:
        return true;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    case SH_Widget_Animate:
        return StyleConfigData::animationsEnabled();
#endif
    case SH_Menu_SupportsSections:
        return true;
    case SH_Widget_Animation_Duration:
        return 150;
    case SH_DialogButtonBox_ButtonsHaveIcons:
        return true;
    case SH_GroupBox_TextLabelVerticalAlignment:
        return Qt::AlignVCenter;
    case SH_TabBar_Alignment:
        return Qt::AlignLeft;
    case SH_ToolBox_SelectedPageTitleBold:
        return false;
    case SH_ScrollBar_MiddleClickAbsolutePosition:
        return true;
    case SH_ScrollView_FrameOnlyAroundContents:
        return false;
    case SH_FormLayoutFormAlignment:
        return Qt::AlignLeft | Qt::AlignTop;
    case SH_FormLayoutLabelAlignment:
        return Qt::AlignRight;
    case SH_FormLayoutFieldGrowthPolicy:
        return QFormLayout::ExpandingFieldsGrow;
    case SH_FormLayoutWrapPolicy:
        return QFormLayout::DontWrapRows;
    case SH_MessageBox_TextInteractionFlags:
        return Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse;
    case SH_ProgressDialog_CenterCancelButton:
        return false;
    case SH_MessageBox_CenterButtons:
        return false;
    case SH_FocusFrame_AboveWidget:
        return true;
    case SH_FocusFrame_Mask:
        return false;
    case SH_RequestSoftwareInputPanel:
        return RSIP_OnMouseClick;
    case SH_TitleBar_NoBorder:
        return true;
    case SH_DockWidget_ButtonsHaveFrame:
        return false;
    default:
        return QCommonStyle::styleHint(hint, option, widget, returnData);
    }
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
    // WA_Hover setup stolen from Breeze
    // enable mouse over effects for all necessary widgets
    if (qobject_cast<QAbstractItemView *>(widget) || qobject_cast<QAbstractSpinBox *>(widget) || qobject_cast<QCheckBox *>(widget)
        || qobject_cast<QComboBox *>(widget) || qobject_cast<QDial *>(widget) || qobject_cast<QLineEdit *>(widget) || qobject_cast<QPushButton *>(widget)
        || qobject_cast<QRadioButton *>(widget) || qobject_cast<QScrollBar *>(widget) || qobject_cast<QSlider *>(widget)
        || qobject_cast<QSplitterHandle *>(widget) || qobject_cast<QTabBar *>(widget) || qobject_cast<QTextEdit *>(widget)
        || qobject_cast<QToolButton *>(widget) || widget->inherits("KTextEditor::View")) {
        widget->setAttribute(Qt::WA_Hover);
    }
    if (auto itemView = qobject_cast<QAbstractItemView *>(widget)) {
        // enable mouse over effects in the viewport of the itemview
        itemView->viewport()->setAttribute(Qt::WA_Hover);

    } else if (auto groupBox = qobject_cast<QGroupBox *>(widget)) {
        // checkable group boxes
        if (groupBox->isCheckable()) {
            groupBox->setAttribute(Qt::WA_Hover);
        }

    } else if (qobject_cast<QAbstractButton *>(widget) && qobject_cast<QDockWidget *>(widget->parent())) {
        widget->setAttribute(Qt::WA_Hover);

    } else if (qobject_cast<QAbstractButton *>(widget) && qobject_cast<QToolBox *>(widget->parent())) {
        widget->setAttribute(Qt::WA_Hover);
    }
    // enable mouse over effect in sunken scrollareas that support focus
    if (auto scrollArea = qobject_cast<QAbstractScrollArea *>(widget)) {
        if (scrollArea->frameShadow() == QFrame::Sunken && scrollArea->focusPolicy() & Qt::StrongFocus) {
            scrollArea->setAttribute(Qt::WA_Hover);
        }
    }

    widget->setProperty(property_union_member_list, setupMemberList(widget));

    QCommonStyle::polish(widget);
}

void UnionStyle::drawText(const QRect &rect, const QStyleOption *opt, QPainter *painter, const QString &text, const QWidget *widget) const
{
    if (text.isEmpty()) {
        return;
    }

    QList<Union::Element::Ptr> elements = prepareElements(opt, widget);
    const bool enabled = opt->state.testFlag(QStyle::State_Enabled);
    auto properties = queryProperties(elements);
    // TODO: hide mnemonics if requested
    auto textColor = properties->text()->color();
    QColor penColor = opt->palette.text().color();
    if (textColor) {
        penColor = textColor->toQColor();
    }

    painter->save();
    painter->setPen(penColor);
    drawItemText(painter, rect, textFlagsFromProperties(properties, true), opt->palette, enabled, text);
    painter->restore();
}

void UnionStyle::drawIcon(const QRect &rect, const QStyleOption *opt, QPainter *painter, const QIcon &icon, const QWidget *widget) const
{
    QList<Union::Element::Ptr> elements = prepareElements(opt, widget);
    const bool enabled = opt->state.testFlag(QStyle::State_Enabled);
    auto properties = queryProperties(elements);

    auto iconColor = properties->icon()->color();
    const QPalette activePalette = opt->palette;
    const qreal dpr = painter->device() ? painter->device()->devicePixelRatioF() : qApp->devicePixelRatio();
    const QPixmap pixmap = icon.pixmap(rect.size(), dpr, enabled ? QIcon::Normal : QIcon::Disabled);
    QColor penColor = opt->palette.text().color(); // Use text color as fallback
    if (iconColor) {
        penColor = iconColor->toQColor();
    }

    painter->save();
    painter->setPen(penColor);
    drawItemPixmap(painter, rect, Qt::AlignCenter, pixmap);
    painter->restore();
}

void UnionStyle::layoutAndDrawIconText(const QStyleOption *opt, QPainter *painter, const QWidget *widget, const QIcon &icon, const QString &text) const
{
    QList<Union::Element::Ptr> elements = prepareElements(opt, widget);

    bool hasIcon = !icon.isNull();
    bool hasText = !text.isEmpty();

    QStringList subElements;
    if (hasIcon) {
        subElements.append(QStringLiteral("Icon"));
    }
    if (hasText) {
        subElements.append(QStringLiteral("Text"));
    }
    // Nothing to draw, just return
    if (subElements.isEmpty()) {
        return;
    }
    auto map = layoutMap(elements, opt, subElements);

    if (hasText) {
        QRect textRect = map[QStringLiteral("Text")].rect.toRect();
        drawText(textRect, opt, painter, text, widget);
    }

    if (hasIcon) {
        QRect iconRect = map[QStringLiteral("Icon")].rect.toRect();
        drawIcon(iconRect, opt, painter, icon, widget);
    }
}
