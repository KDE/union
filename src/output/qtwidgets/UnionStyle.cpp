// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "UnionStyle.h"
#include "BackgroundDrawing.h"
#include "SharedNames.h"
#include "StyleUtils.h"
#include "elements/AbstractElement.h"
#include "elements/ButtonElement.h"
#include "elements/CheckBoxElement.h"
#include "elements/ComboBoxElement.h"
#include "elements/DockWidgetElement.h"
#include "elements/FocusElement.h"
#include "elements/FrameElement.h"
#include "elements/GroupBoxElement.h"
#include "elements/HeaderElement.h"
#include "elements/IndicatorElement.h"
#include "elements/ItemViewElement.h"
#include "elements/LineEditElement.h"
#include "elements/MenuBarElement.h"
#include "elements/MenuBarItemElement.h"
#include "elements/MenuElement.h"
#include "elements/MenuItemElement.h"
#include "elements/ProgressBarElement.h"
#include "elements/RadioButtonElement.h"
#include "elements/RubberBandElement.h"
#include "elements/ScrollAreaCornerElement.h"
#include "elements/ScrollBarElement.h"
#include "elements/SizeGripElement.h"
#include "elements/SliderElement.h"
#include "elements/SpinBoxElement.h"
#include "elements/SplitterElement.h"
#include "elements/StatusBarElement.h"
#include "elements/TabBarElement.h"
#include "elements/TabCloseButtonElement.h"
#include "elements/TabElement.h"
#include "elements/TabWidgetElement.h"
#include "elements/TitleBarElement.h"
#include "elements/ToolBarElement.h"
#include "elements/ToolBoxTabElement.h"
#include "elements/ToolButtonElement.h"
#include "elements/ToolTipElement.h"
#include "elements/TreeViewElement.h"
#include "elements/WidgetElement.h"

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
#include <QRubberBand>
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

#include "ElementCache.h"

using namespace Qt::StringLiterals;

UnionStyle::UnionStyle()
    : QCommonStyle()
    , m_showMnemonics(false)
{
    qApp->installEventFilter(this);
    Union::StyleRegistry::instance()->load();
}

UnionStyle::~UnionStyle() noexcept
{
    ElementCache::clear();
}

void UnionStyle::drawControl(QStyle::ControlElement controlElement, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // Make lines not look completely terrible on fractional scales
    painter->setRenderHint(QPainter::Antialiasing, true);
    const auto hash = qHashMulti(QHashSeed::globalSeed(), controlElement, option, widget);
    auto drawEnums = DrawEnums();
    drawEnums.ControlElement = controlElement;
    auto cached = ElementCache::element<QStyle::ControlElement,
                                        QStyle::CE_PushButton,
                                        QStyle::CE_PushButtonBevel,
                                        QStyle::CE_PushButtonLabel,
                                        QStyle::CE_CheckBox,
                                        QStyle::CE_CheckBoxLabel,
                                        QStyle::CE_RadioButton,
                                        QStyle::CE_RadioButtonLabel,
                                        QStyle::CE_TabBarTab,
                                        QStyle::CE_TabBarTabShape,
                                        QStyle::CE_TabBarTabLabel,
                                        QStyle::CE_ProgressBar,
                                        QStyle::CE_ProgressBarGroove,
                                        QStyle::CE_ProgressBarContents,
                                        QStyle::CE_ProgressBarLabel,
                                        QStyle::CE_MenuItem,
                                        QStyle::CE_MenuBarItem,
                                        QStyle::CE_MenuBarEmptyArea,
                                        QStyle::CE_ToolButtonLabel,
                                        QStyle::CE_Header,
                                        QStyle::CE_HeaderSection,
                                        QStyle::CE_HeaderLabel,
                                        QStyle::CE_ToolBoxTab,
                                        QStyle::CE_SizeGrip,
                                        QStyle::CE_Splitter,
                                        QStyle::CE_RubberBand,
                                        QStyle::CE_DockWidgetTitle,
                                        QStyle::CE_ScrollBarAddLine,
                                        QStyle::CE_ScrollBarSubLine,
                                        QStyle::CE_ScrollBarAddPage,
                                        QStyle::CE_ScrollBarSubPage,
                                        QStyle::CE_ScrollBarSlider,
                                        QStyle::CE_ScrollBarFirst,
                                        QStyle::CE_ScrollBarLast,
                                        QStyle::CE_FocusFrame,
                                        QStyle::CE_ComboBoxLabel,
                                        QStyle::CE_ToolBar,
                                        QStyle::CE_ToolBoxTabShape,
                                        QStyle::CE_ToolBoxTabLabel,
                                        QStyle::CE_HeaderEmptyArea,
                                        QStyle::CE_ItemViewItem,
                                        QStyle::CE_ShapedFrame>(controlElement, hash, this, option, widget);
    if (cached) {
        cached->draw(painter, drawEnums);
    } else {
        // Unimplemented
        // CE_MenuScroller
        // CE_MenuTearoff
        // CE_MenuEmptyArea
        // CE_MenuVMargin
        // CE_MenuHMargin
        // CE_ColumnViewGrip
        QCommonStyle::drawControl(controlElement, option, painter, widget);
    }
}

void UnionStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    // Make lines not look completely terrible on fractional scales
    painter->setRenderHint(QPainter::Antialiasing, true);
    const auto hash = qHashMulti(QHashSeed::globalSeed(), control, option, widget);
    auto drawEnums = DrawEnums();
    drawEnums.ComplexControl = control;

    auto cached = ElementCache::element<QStyle::ComplexControl,
                                        QStyle::CC_ToolButton,
                                        QStyle::CC_SpinBox,
                                        QStyle::CC_ComboBox,
                                        QStyle::CC_ScrollBar,
                                        QStyle::CC_Slider,
                                        QStyle::CC_TitleBar,
                                        QStyle::CC_GroupBox>(control, hash, this, option, widget);
    if (cached) {
        cached->draw(painter, drawEnums);
    } else {
        // TODO: Dial, MDI controls
        QCommonStyle::drawComplexControl(control, option, painter, widget);
    }
}

QStyle::SubControl
UnionStyle::hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option, const QPoint &point, const QWidget *widget) const
{
    switch (control) {
    // Make scrollbar behave like in QtQuick
    case CC_ScrollBar: {
        auto grooveRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarGroove, widget);
        if (grooveRect.contains(point)) {
            const auto sliderRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarSlider, widget);
            const auto precedes = [](const QStyleOptionComplex *option, QPoint point, QRectF rect) {
                if (option->state & QStyle::State_Horizontal) {
                    if (option->direction == Qt::LeftToRight) {
                        return point.x() < rect.right();
                    } else {
                        return point.x() > rect.x();
                    }
                } else {
                    return point.y() < rect.y();
                }
            };
            if (sliderRect.contains(point)) {
                return SC_ScrollBarSlider;
            } else if (precedes(option, point, sliderRect)) {
                return SC_ScrollBarSubPage;
            } else {
                return SC_ScrollBarAddPage;
            }
        }
    }
    default:
        return QCommonStyle::hitTestComplexControl(control, option, point, widget);
    }
}

void UnionStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // Make lines not look completely terrible on fractional scales
    painter->setRenderHint(QPainter::Antialiasing, true);
    const auto hash = qHashMulti(QHashSeed::globalSeed(), element, option, widget);
    auto drawEnums = DrawEnums();
    drawEnums.PrimitiveElement = element;
    auto cached = ElementCache::element<QStyle::PrimitiveElement,
                                        QStyle::PE_Frame,
                                        QStyle::PE_FrameDefaultButton,
                                        QStyle::PE_FrameDockWidget,
                                        QStyle::PE_FrameFocusRect,
                                        QStyle::PE_FrameGroupBox,
                                        QStyle::PE_FrameLineEdit,
                                        QStyle::PE_FrameMenu,
                                        QStyle::PE_FrameStatusBarItem,
                                        QStyle::PE_FrameTabWidget,
                                        QStyle::PE_FrameWindow,
                                        QStyle::PE_FrameButtonBevel,
                                        QStyle::PE_FrameButtonTool,
                                        QStyle::PE_FrameTabBarBase,
                                        QStyle::PE_PanelButtonCommand,
                                        QStyle::PE_PanelButtonBevel,
                                        QStyle::PE_PanelButtonTool,
                                        QStyle::PE_PanelMenuBar,
                                        QStyle::PE_PanelToolBar,
                                        QStyle::PE_PanelLineEdit,
                                        QStyle::PE_IndicatorArrowDown,
                                        QStyle::PE_IndicatorArrowLeft,
                                        QStyle::PE_IndicatorArrowRight,
                                        QStyle::PE_IndicatorArrowUp,
                                        QStyle::PE_IndicatorBranch,
                                        QStyle::PE_IndicatorButtonDropDown,
                                        QStyle::PE_IndicatorCheckBox,
                                        QStyle::PE_IndicatorHeaderArrow,
                                        QStyle::PE_IndicatorProgressChunk,
                                        QStyle::PE_IndicatorRadioButton,
                                        QStyle::PE_IndicatorSpinDown,
                                        QStyle::PE_IndicatorSpinMinus,
                                        QStyle::PE_IndicatorSpinPlus,
                                        QStyle::PE_IndicatorSpinUp,
                                        QStyle::PE_IndicatorToolBarHandle,
                                        QStyle::PE_IndicatorToolBarSeparator,
                                        QStyle::PE_PanelTipLabel,
                                        QStyle::PE_PanelScrollAreaCorner,
                                        QStyle::PE_Widget,
                                        QStyle::PE_PanelItemViewItem,
                                        QStyle::PE_PanelStatusBar,
                                        QStyle::PE_IndicatorTabClose,
                                        QStyle::PE_PanelMenu>(element, hash, this, option, widget);
    if (cached) {
        cached->draw(painter, drawEnums);
    } else {
        // Handle some items that are just duplicates of the above, or need fallback
        switch (element) {
        case QStyle::PE_IndicatorMenuCheckMark:
        case QStyle::PE_IndicatorItemViewItemCheck:
            drawPrimitive(PE_IndicatorCheckBox, option, painter, widget);
            return;
        case QStyle::PE_IndicatorHeaderArrow:
            // Fallback
            if (option->state.testFlags(State_UpArrow)) {
                drawPrimitive(PE_IndicatorArrowUp, option, painter, widget);
            } else if (option->state.testFlags(State_DownArrow)) {
                drawPrimitive(PE_IndicatorArrowDown, option, painter, widget);
            }
            return;
        case QStyle::PE_IndicatorColumnViewArrow:
            drawPrimitive(PE_IndicatorArrowRight, option, painter, widget);
            return;
        default:
            break;
            // Unhandled:
            // QStyle::PE_PanelItemViewRow
            // QStyle::PE_IndicatorTabTear
            // QStyle::PE_IndicatorTabTearRight
            // QStyle::PE_IndicatorItemViewItemDrop
            // QStyle::PE_IndicatorDockWidgetResizeHandle
            // QStyle::PE_CustomBase
        }
        QCommonStyle::drawPrimitive(element, option, painter, widget);
    }
}

QSize UnionStyle::sizeFromContents(QStyle::ContentsType contentsType, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget) const
{
    const auto hash = qHashMulti(QHashSeed::globalSeed(), contentsType, option, contentsSize, widget);
    auto cached = ElementCache::element<QStyle::ContentsType,
                                        QStyle::CT_PushButton,
                                        QStyle::CT_CheckBox,
                                        QStyle::CT_RadioButton,
                                        QStyle::CT_ToolButton,
                                        QStyle::CT_ComboBox,
                                        QStyle::CT_Splitter,
                                        QStyle::CT_ProgressBar,
                                        QStyle::CT_MenuItem,
                                        QStyle::CT_MenuBarItem,
                                        QStyle::CT_MenuBar,
                                        QStyle::CT_Menu,
                                        QStyle::CT_TabBarTab,
                                        QStyle::CT_Slider,
                                        QStyle::CT_ScrollBar,
                                        QStyle::CT_LineEdit,
                                        QStyle::CT_SpinBox,
                                        QStyle::CT_SizeGrip,
                                        QStyle::CT_TabWidget,
                                        QStyle::CT_HeaderSection,
                                        QStyle::CT_GroupBox,
                                        QStyle::CT_ItemViewItem>(contentsType, hash, this, option, widget);
    if (cached) {
        return cached->contentsSize(contentsSize).toSize();
    } else {
        // Unhandled
        // QStyle::CT_DialogButtons:
        // QStyle::CT_MdiControls:
        // QStyle::CT_CustomBase:
        return QCommonStyle::sizeFromContents(contentsType, option, contentsSize, widget);
    }
}

QRect UnionStyle::subElementRect(QStyle::SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    const auto hash = qHashMulti(QHashSeed::globalSeed(), element, option, widget);

    auto cached = ElementCache::element<QStyle::SubElement,
                                        QStyle::SE_ItemViewItemText,
                                        QStyle::SE_ItemViewItemDecoration,
                                        QStyle::SE_ItemViewItemCheckIndicator,
                                        QStyle::SE_RadioButtonContents,
                                        QStyle::SE_RadioButtonIndicator,
                                        QStyle::SE_CheckBoxIndicator,
                                        QStyle::SE_CheckBoxContents,
                                        QStyle::SE_PushButtonFocusRect,
                                        QStyle::SE_PushButtonContents,
                                        QStyle::SE_PushButtonBevel,
                                        QStyle::SE_LineEditContents,
                                        QStyle::SE_HeaderArrow,
                                        QStyle::SE_HeaderLabel,
                                        QStyle::SE_TabBarTabText,
                                        QStyle::SE_ProgressBarLabel,
                                        QStyle::SE_ProgressBarContents,
                                        QStyle::SE_ProgressBarGroove,
                                        QStyle::SE_DockWidgetTitleBarText,
                                        QStyle::SE_DockWidgetCloseButton,
                                        QStyle::SE_DockWidgetFloatButton,
                                        QStyle::SE_DockWidgetIcon,
                                        QStyle::SE_ToolBoxTabContents>(element, hash, this, option, widget);
    if (cached) {
        return cached->subElementRect(element).toRect();
    }

    return QCommonStyle::subElementRect(element, option, widget);
}

QRect UnionStyle::subControlRect(ComplexControl complexControl, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget) const
{
    const auto hash = qHashMulti(QHashSeed::globalSeed(), complexControl, option, subControl, widget);

    auto cached = ElementCache::element<QStyle::ComplexControl,
                                        QStyle::CC_ToolButton,
                                        QStyle::CC_SpinBox,
                                        QStyle::CC_ComboBox,
                                        QStyle::CC_ScrollBar,
                                        QStyle::CC_Slider,
                                        QStyle::CC_TitleBar,
                                        QStyle::CC_GroupBox>(complexControl, hash, this, option, widget);
    if (cached) {
        return cached->subControlRect(subControl).toRect();
    }
    // TODO: Leave Dial and MDIControls to QCommonStyle for now
    return QCommonStyle::subControlRect(complexControl, option, subControl, widget);
}

int UnionStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    const auto hash = qHashMulti(QHashSeed::globalSeed(), metric, option, widget);
    auto cached = ElementCache::element<QStyle::PixelMetric,
                                        QStyle::PM_ButtonMargin,
                                        QStyle::PM_ButtonDefaultIndicator,
                                        QStyle::PM_MenuButtonIndicator,
                                        QStyle::PM_DefaultFrameWidth,
                                        QStyle::PM_SpinBoxFrameWidth,
                                        QStyle::PM_ComboBoxFrameWidth,
                                        QStyle::PM_ScrollBarExtent,
                                        QStyle::PM_ScrollBarSliderMin,
                                        QStyle::PM_SliderThickness,
                                        QStyle::PM_SliderControlThickness,
                                        QStyle::PM_SliderLength,
                                        QStyle::PM_DockWidgetFrameWidth,
                                        QStyle::PM_TabBarTabHSpace,
                                        QStyle::PM_TabBarTabVSpace,
                                        QStyle::PM_TabBarBaseHeight,
                                        QStyle::PM_ProgressBarChunkWidth,
                                        QStyle::PM_SplitterWidth,
                                        QStyle::PM_TitleBarHeight,
                                        QStyle::PM_MenuHMargin,
                                        QStyle::PM_MenuVMargin,
                                        QStyle::PM_MenuPanelWidth,
                                        QStyle::PM_MenuDesktopFrameWidth,
                                        QStyle::PM_MenuBarPanelWidth,
                                        QStyle::PM_MenuBarItemSpacing,
                                        QStyle::PM_MenuBarVMargin,
                                        QStyle::PM_MenuBarHMargin,
                                        QStyle::PM_IndicatorWidth,
                                        QStyle::PM_IndicatorHeight,
                                        QStyle::PM_ExclusiveIndicatorWidth,
                                        QStyle::PM_ExclusiveIndicatorHeight,
                                        QStyle::PM_HeaderMargin,
                                        QStyle::PM_HeaderMarkSize,
                                        QStyle::PM_TabBarScrollButtonWidth,
                                        QStyle::PM_ToolBarFrameWidth,
                                        QStyle::PM_ToolBarHandleExtent,
                                        QStyle::PM_ToolBarItemSpacing,
                                        QStyle::PM_ToolBarItemMargin,
                                        QStyle::PM_ToolBarSeparatorExtent,
                                        QStyle::PM_ToolBarExtensionExtent,
                                        QStyle::PM_ToolBarIconSize,
                                        QStyle::PM_ListViewIconSize,
                                        QStyle::PM_IconViewIconSize,
                                        QStyle::PM_SmallIconSize,
                                        QStyle::PM_LargeIconSize,
                                        QStyle::PM_FocusFrameVMargin,
                                        QStyle::PM_FocusFrameHMargin,
                                        QStyle::PM_ToolTipLabelFrameWidth,
                                        QStyle::PM_CheckBoxLabelSpacing,
                                        QStyle::PM_TabBarIconSize,
                                        QStyle::PM_SizeGripSize,
                                        QStyle::PM_MessageBoxIconSize,
                                        QStyle::PM_ButtonIconSize,
                                        QStyle::PM_RadioButtonLabelSpacing,
                                        QStyle::PM_TextCursorWidth,
                                        QStyle::PM_TabCloseIndicatorWidth,
                                        QStyle::PM_TabCloseIndicatorHeight,
                                        QStyle::PM_TreeViewIndentation,
                                        QStyle::PM_HeaderDefaultSectionSizeHorizontal,
                                        QStyle::PM_HeaderDefaultSectionSizeVertical,
                                        QStyle::PM_TitleBarButtonIconSize,
                                        QStyle::PM_TitleBarButtonSize,
                                        QStyle::PM_LineEditIconSize,
                                        QStyle::PM_LineEditIconMargin>(metric, hash, this, option, widget);
    if (cached) {
        return cached->pixelMetric(metric);
    }
    // Unimplemented, use QCommonStyle for now
    // QStyle::PM_MenuScrollerHeight
    // QStyle::PM_SpinBoxSliderHeight
    // QStyle::PM_ScrollView_ScrollBarSpacing
    // QStyle::PM_MdiSubWindowFrameWidth
    // QStyle::PM_MaximumDragDistance
    // QStyle::PM_SliderTickmarkOffset
    // QStyle::PM_SliderSpaceAvailable
    // QStyle::PM_DockWidgetSeparatorExtent
    // QStyle::PM_DockWidgetHandleExtent
    // QStyle::PM_TabBarTabOverlap
    // QStyle::PM_TabBarBaseOverlap
    // QStyle::PM_MenuTearoffHeight
    // QStyle::PM_DialogButtonsSeparator // Deprecated
    // QStyle::PM_DialogButtonsButtonWidth // Deprecated
    // QStyle::PM_DialogButtonsButtonHeight // Deprecated
    // QStyle::PM_MdiSubWindowMinimizedWidth
    // QStyle::PM_HeaderGripMargin
    // QStyle::PM_DockWidgetTitleMargin
    // QStyle::PM_DockWidgetTitleBarButtonMargin
    // QStyle::PM_SubMenuOverlap
    // QStyle::PM_CustomBase
    // QStyle::PM_LayoutHorizontalSpacing
    // QStyle::PM_LayoutVerticalSpacing
    // QStyle::PM_LayoutLeftMargin
    // QStyle::PM_LayoutTopMargin
    // QStyle::PM_LayoutRightMargin
    // QStyle::PM_LayoutBottomMargin
    // Overrides
    switch (metric) {
    // Don't shift button text when sunken
    case QStyle::PM_TabBarTabShiftHorizontal:
    case QStyle::PM_TabBarTabShiftVertical:
    case QStyle::PM_ButtonShiftHorizontal:
    case QStyle::PM_ButtonShiftVertical:
    case QStyle::PM_TabBar_ScrollButtonOverlap:
    // Don't allow overlap
    case QStyle::PM_ScrollView_ScrollBarOverlap:
        return 0;
    default:
        break;
    }
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
    case SH_ScrollBar_LeftClickAbsolutePosition:
        return true;
    case SH_UnderlineShortcut:
        return true;
    default:
        return QCommonStyle::styleHint(hint, option, widget, returnData);
    }
}

void UnionStyle::polish(QApplication *application)
{
    QCommonStyle::polish(application);

    // Set global window color
    auto element = Union::Element::create();
    element->setType(ElementString::ApplicationWindow);

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
    if (qobject_cast<QScrollBar *>(widget)) {
        // remove opaque painting for scrollbars
        widget->setAttribute(Qt::WA_OpaquePaintEvent, false);
    }

    widget->setProperty(property_union_member_list, widgetToElementHierarchy(widget));

    QCommonStyle::polish(widget);
}

void UnionStyle::drawItemText(QPainter *painter,
                              const QRect &rect,
                              int flags,
                              const QPalette &pal,
                              bool enabled,
                              const QString &text,
                              QPalette::ColorRole textRole) const
{
    flags |= m_showMnemonics ? Qt::TextShowMnemonic : Qt::TextHideMnemonic;
    QCommonStyle::drawItemText(painter, rect, flags, pal, enabled, text, textRole);
}

void UnionStyle::setMnemonics(bool enabled)
{
    if (m_showMnemonics != enabled) {
        m_showMnemonics = enabled;
        const auto widgets = qApp->topLevelWidgets();
        for (QWidget *widget : widgets) {
            widget->update();
        }
    }
}

bool UnionStyle::eventFilter(QObject *object, QEvent *event)
{
    switch (event->type()) {
    case QEvent::KeyPress:
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Alt) {
            setMnemonics(true);
        }
        break;

    case QEvent::KeyRelease:
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Alt) {
            setMnemonics(false);
        }
        break;

    case QEvent::ApplicationStateChange:
        setMnemonics(false);
        break;

    default:
        break;
    }
    return QCommonStyle::eventFilter(object, event);
}

QIcon UnionStyle::unionIcon(Union::Properties::StylePropertyGroup *properties, const QString &defaultName) const
{
    if (properties && properties->icon()) {
        auto name = properties->icon()->name().value_or(defaultName);
        if (!name.isEmpty()) {
            QColor color = standardPalette().text().color();
            if (properties->icon()->color()) {
                color = properties->icon()->color()->toQColor();
            }
            return Union::StyleRegistry::instance()->platform()->platformIcon(name, color);
        }
    }
    return QIcon::fromTheme(defaultName);
}
