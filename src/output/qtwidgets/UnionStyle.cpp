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

    if (controlElement == CE_PushButton) {
        const auto buttonOption = static_cast<const QStyleOptionButton *>(option);

        auto element = Union::Element::create();
        element->setType(QStringLiteral("Button"));
        element->setStates(statesFromOption(option));
        element->setColorSet(colorsetFromOption(option));
        element->setHints(hintsFromOption(option));

        const auto properties = prepareProperties(element);
        auto rect = prepareRectangle(option, properties).toRect();
        drawBackground(painter, rect, properties);

        QStyleOptionButton labelOption(*buttonOption);
        labelOption.palette.setColor(QPalette::ButtonText, properties->text()->color().value().toQColor());

        QProxyStyle::drawControl(CE_PushButtonLabel, &labelOption, painter, widget);

        return;
    }
    QProxyStyle::drawControl(controlElement, option, painter, widget);
}

void UnionStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    if (control == CC_ToolButton) {
        const auto buttonOption = static_cast<const QStyleOptionToolButton *>(option);

        auto element = Union::Element::create();
        element->setType(QStringLiteral("ToolButton"));
        element->setStates(statesFromOption(option));
        element->setColorSet(colorsetFromOption(option));
        bool flat = option->state & QStyle::State_AutoRaise;
        element->setHint(QStringLiteral("raised"), !flat);

        const auto properties = prepareProperties(element);
        auto rect = prepareRectangle(option, properties).toRect();
        drawBackground(painter, rect, properties);

        QStyleOptionToolButton labelOption(*buttonOption);
        labelOption.palette.setColor(QPalette::ButtonText, properties->text()->color().value().toQColor());

        QProxyStyle::drawControl(CE_ToolButtonLabel, &labelOption, painter, widget);
        return;
    }
    if (control == CC_GroupBox) {
        const auto groupBoxOption = static_cast<const QStyleOptionGroupBox *>(option);

        auto element = Union::Element::create();
        element->setType(QStringLiteral("GroupBox"));
        element->setStates(statesFromOption(option));
        element->setColorSet(colorsetFromOption(option));

        const auto properties = prepareProperties(element);
        auto rect = prepareRectangle(option, properties).toRect();
        drawBackground(painter, rect, properties);
        if ((groupBoxOption->subControls & QStyle::SC_GroupBoxLabel) && !groupBoxOption->text.isEmpty()) {
            QRect textRect = subControlRect(CC_GroupBox, option, SC_GroupBoxLabel, widget);
            QColor textColor = properties->text()->color().value().toQColor();

            painter->setPen(textColor);
            drawItemText(painter,
                         textRect,
                         Qt::TextShowMnemonic | Qt::AlignHCenter | groupBoxOption->textAlignment,
                         groupBoxOption->palette,
                         groupBoxOption->state & State_Enabled,
                         groupBoxOption->text,
                         textColor.isValid() ? QPalette::NoRole : QPalette::WindowText);
        }
        return;
    }

    QProxyStyle::drawComplexControl(control, option, painter, widget);
}

void UnionStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case QStyle::PE_FrameStatusBarItem:
        drawElement(option, painter, widget, QStringLiteral("Item"));
        return;
    case QStyle::PE_Widget:
        // Relates to PE_Frame
        drawElement(option, painter, widget, QStringLiteral("Panel"));
        return;
        // Standalone elements
    case QStyle::PE_PanelLineEdit:
        // For spinboxes and comboboxes, we do not want to draw this element
        // TODO: remove if this is not needed after making the complex controls
        if (widget->parentWidget()->inherits("QComboBox") || widget->parentWidget()->inherits("QAbstractSpinBox")) {
            return;
        }
        drawElement(option, painter, widget);
        return;
    case QStyle::PE_PanelItemViewItem:
        drawElement(option, painter, widget, QStringLiteral("ItemViewItem"));
        return;
    case QStyle::PE_PanelItemViewRow:
        drawElement(option, painter, widget, QStringLiteral("ItemViewRow"));
        return;
    case QStyle::PE_PanelScrollAreaCorner:
        drawElement(option, painter, widget, QStringLiteral("ScrollAreaCorner"));
        return;
    case QStyle::PE_PanelTipLabel:
        drawElement(option, painter, widget, QStringLiteral("Text"));
        return;
    case QStyle::PE_FrameFocusRect:
        drawElement(option, painter, widget, QStringLiteral("FocusFrame"));
        return;
        // Indicators
    case QStyle::PE_IndicatorCheckBox:
        drawElement(option, painter, widget, QStringLiteral("Indicator"));
        return;
    case QStyle::PE_IndicatorRadioButton:
        drawElement(option, painter, widget, QStringLiteral("Indicator"));
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
        drawElement(option, painter, widget);
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
    palette.setColor(QPalette::Window, properties->background()->color().value().toQColor());

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
