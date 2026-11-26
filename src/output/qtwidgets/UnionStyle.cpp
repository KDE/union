// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "UnionStyle.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMenu>
#include <QScrollBar>

#include <StyleRegistry.h>

#include "ElementCache.h"
#include "SharedNames.h"
#include "StyleUtils.h"
#include "UnionGraphicsEffect.h"

#include "elements/AbstractElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;
using namespace Union::Widgets;

UnionStyle::UnionStyle()
    : QCommonStyle()
    , m_dragFilter(std::make_unique<DragEventFilter>(this))
{
    qApp->installEventFilter(this);
    Union::StyleRegistry::instance()->load();
}

UnionStyle::~UnionStyle() noexcept
{
    ElementCache::clear();
}

AbstractElement *UnionStyle::element(const QWidget *widget, const QStyleOption *option) const
{
    // Always use the widget if we have it.
    if (widget) {
        return widget->property(UnionElementProperty).value<AbstractElement *>();
    }

    // If we have neither widget nor option, we can't do anything useful here.
    if (!option) {
        return nullptr;
    }

    // We might be able to get the widget from the option instead, if so, use that.
    if (auto w = qobject_cast<QWidget *>(option->styleObject)) {
        return w->property(UnionElementProperty).value<AbstractElement *>();
    }

    if (m_optionElementCache.contains(option->type)) {
        return m_optionElementCache.value(option->type).value().get();
    }

    auto element = TypeMapping::createElementForOption(this, option->type);
    m_optionElementCache.insert(option->type, std::shared_ptr<AbstractElement>(element));
    return element;
}

void UnionStyle::drawControl(QStyle::ControlElement controlElement, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto e = element(widget, option);
    if (e) {
        e->update(option);
        e->draw(painter, {controlElement});
        return;
    }

    switch (controlElement) {
    case CE_MenuEmptyArea:
    case CE_MenuVMargin:
    case CE_MenuHMargin:
        return;
        // Unimplemented
        // CE_MenuScroller
        // CE_MenuTearoff
        // CE_ColumnViewGrip
    default:
        QCommonStyle::drawControl(controlElement, option, painter, widget);
        break;
    }
}

void UnionStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    auto e = element(widget, option);
    if (e) {
        e->update(option);
        e->draw(painter, {control});
        return;
    }

    QCommonStyle::drawComplexControl(control, option, painter, widget);
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
            const auto precedes = [](const QStyleOptionComplex *option, QPoint point, const QRectF &rect) {
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
        break;
    }
    default:
        break;
    }
    return QCommonStyle::hitTestComplexControl(control, option, point, widget);
}

void UnionStyle::drawPrimitive(QStyle::PrimitiveElement primitive, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto e = element(widget, option);
    if (e) {
        e->update(option);
        e->draw(painter, {primitive});
        return;
    }

    // Handle some items that are just duplicates of the above, or need fallback
    switch (primitive) {
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
    QCommonStyle::drawPrimitive(primitive, option, painter, widget);
}

QSize UnionStyle::sizeFromContents(QStyle::ContentsType contentsType, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget) const
{
    auto e = element(widget, option);
    if (e) {
        e->update(option);
        return e->contentsSize(option, contentsSize).toSize();
    }

    // Unhandled
    // QStyle::CT_DialogButtons:
    // QStyle::CT_MdiControls:
    // QStyle::CT_CustomBase:
    return QCommonStyle::sizeFromContents(contentsType, option, contentsSize, widget);
}

QRect UnionStyle::subElementRect(QStyle::SubElement subElement, const QStyleOption *option, const QWidget *widget) const
{
    auto e = element(widget, option);
    if (e) {
        e->update(option);
        auto rect = e->subElementRect(option, subElement);
        if (rect) {
            return rect->toRect();
        } else {
            qCDebug(UNION_QTWIDGETS) << "SubElement" << subElement << "not handled by element" << e;
        }
    }

    return QCommonStyle::subElementRect(subElement, option, widget);
}

QRect UnionStyle::subControlRect(ComplexControl complexControl, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget) const
{
    auto e = element(widget, option);
    if (e) {
        e->update(option);
        auto rect = e->subControlRect(option, subControl);
        if (rect) {
            return rect->toRect();
        } else {
            qCDebug(UNION_QTWIDGETS) << "SubControl" << subControl << "not handled by element" << e;
        }
    }

    // TODO: Leave Dial and MDIControls to QCommonStyle for now
    return QCommonStyle::subControlRect(complexControl, option, subControl, widget);
}

int UnionStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    auto e = element(widget, option);
    if (e) {
        e->update(option);
        if (auto value = e->pixelMetric(option, metric); value) {
            return *value;
        } else {
            qCDebug(UNION_QTWIDGETS) << "Pixel metric" << metric << "not handled by element" << e;
        }
    }

    // Unimplemented, use QCommonStyle for now
    // QStyle::PM_MenuScrollerHeight
    // QStyle::PM_SpinBoxSliderHeight
    // QStyle::PM_ScrollView_ScrollBarSpacing
    // QStyle::PM_MdiSubWindowFrameWidth
    // QStyle::PM_MaximumDragDistance
    // QStyle::PM_SliderTickmarkOffset
    // QStyle::PM_SliderSpaceAvailable
    // QStyle::PM_DockWidgetHandleExtent
    // QStyle::PM_TabBarTabOverlap
    // QStyle::PM_TabBarBaseOverlap
    // QStyle::PM_MenuTearoffHeight
    // QStyle::PM_DialogButtonsSeparator // Deprecated
    // QStyle::PM_DialogButtonsButtonWidth // Deprecated
    // QStyle::PM_DialogButtonsButtonHeight // Deprecated
    // QStyle::PM_MdiSubWindowMinimizedWidth
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
        // If no QStyleOption is provided, return 1 so that we do not
        // take extra space when there is no need, but we have click area for the mouse.
    case QStyle::PM_DockWidgetSeparatorExtent:
        return 1;
    // Don't shift button text when sunken
    case QStyle::PM_TabBarTabShiftHorizontal:
    case QStyle::PM_TabBarTabShiftVertical:
    case QStyle::PM_ButtonShiftHorizontal:
    case QStyle::PM_ButtonShiftVertical:
    case QStyle::PM_TabBar_ScrollButtonOverlap:
    // Don't allow overlap
    case QStyle::PM_ScrollView_ScrollBarOverlap:
    case QStyle::PM_MenuBarPanelWidth:
        return 0;
    default:
        break;
    }
    return QCommonStyle::pixelMetric(metric, option, widget);
}

QIcon UnionStyle::standardIcon(QStyle::StandardPixmap pixmap, const QStyleOption *option, const QWidget *widget) const
{
    auto e = element(widget, option);
    if (e) {
        e->update(option);
        return e->standardPixmap(option, pixmap);
    }

    return QCommonStyle::standardIcon(pixmap, option, widget);
}

// Copied from Breeze.
// TODO: Make these adjustable!
int UnionStyle::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    auto e = element(widget, option);
    if (e) {
        e->update(option);
        return e->styleHint(option, hint);
    }

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
                && qobject_cast<const QAbstractItemView *>(widget->parent()->parent())->viewport() == widget->parent()) {
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
        // 0 when the animation speed is set to instantaneous, which turns the animations off.
        return qRound(150 * Union::StyleRegistry::instance()->platform()->animationSpeedMultiplier());
    case SH_DialogButtonBox_ButtonsHaveIcons:
        return true;
    case SH_GroupBox_TextLabelVerticalAlignment:
        return Qt::AlignVCenter;
    case SH_ToolBox_SelectedPageTitleBold:
        return false;
    case SH_ScrollBar_MiddleClickAbsolutePosition:
        return true;
    case SH_ScrollView_FrameOnlyAroundContents:
        return false;
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
        return m_showMnemonics;
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
    if (matches.isEmpty()) {
        return;
    }

    const auto properties = matches.first()->properties();

    QPalette palette;
    if (properties->background() && properties->background()->color()) {
        palette.setColor(QPalette::Window, properties->background()->color().value().toQColor());
    }

    application->setPalette(palette);
}

void UnionStyle::polish(QWidget *widget)
{
    m_dragFilter->installWidget(widget);

    if (widget) {
        // enable mouse over effects for all necessary widgets
        widget->setAttribute(Qt::WA_Hover);

        auto e = element(widget, nullptr);
        if (!e) {
            e = TypeMapping::createElementForWidget(this, widget);
            widget->setProperty(UnionElementProperty, QVariant::fromValue(e));
        }

        if (widget->inherits("QComboBoxPrivateContainer")) {
            widget->setAttribute(Qt::WA_TranslucentBackground);
        }
        // Used to avoid korners bug, as the setAutoFillBackground is not rounded,
        // but the frame around it may be rounded.
        if (widget->parent()) {
            // Do not draw unnecessary background for QComboBoxListView
            if (widget->parent()->inherits("QComboBoxListView")) {
                widget->setAttribute(Qt::WA_TranslucentBackground);
                widget->setAutoFillBackground(false);
            }
        }

        // Avoid korners bug with spinboxes
        if (auto spinBox = qobject_cast<QAbstractSpinBox *>(widget)) {
            spinBox->setAutoFillBackground(false);
        }
    }
    if (qobject_cast<QScrollBar *>(widget)) {
        // remove opaque painting for scrollbars
        widget->setAttribute(Qt::WA_OpaquePaintEvent, false);
    }
    if (qobject_cast<QMenu *>(widget)) {
        widget->setAttribute(Qt::WA_TranslucentBackground);
    }

    QCommonStyle::polish(widget);
}

void UnionStyle::unpolish(QWidget *widget)
{
    m_dragFilter->uninstallWidget(widget);
    QCommonStyle::unpolish(widget);
}

void UnionStyle::drawItemPixmap(QPainter *painter, const QRectF &rect, Qt::Alignment alignment, const QPixmap &pixmap) const
{
    // Try to align to physical pixels.
    auto dpr = pixmap.devicePixelRatio();
    QRectF pixmapRect = QRectF{rect.topLeft() * dpr, rect.size() * dpr}.toRect();
    QRectF aligned = alignedRect(alignment, pixmap.size(), pixmapRect).toRect();
    pixmapRect = aligned.intersected(pixmapRect);
    auto sourceRect = pixmapRect.translated(-aligned.topLeft());
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawPixmap({pixmapRect.topLeft() / dpr, pixmapRect.size() / dpr}, pixmap, sourceRect);
}

void UnionStyle::drawItemPixmap(QPainter *painter, const QRect &rect, int alignmentInt, const QPixmap &pixmap) const
{
    Qt::Alignment alignment{alignmentInt};
    // It would be more correct to use QStyleOption::direction, but we don't have that here.
    alignment = visualAlignment(QGuiApplication::layoutDirection(), alignment);
    drawItemPixmap(painter, QRectF{rect}, alignment, pixmap);
}

void UnionStyle::drawItemText(QPainter *painter,
                              const QRect &rect,
                              int flags,
                              const QPalette &pal,
                              bool enabled,
                              const QString &text,
                              QPalette::ColorRole textRole) const
{
    // Only toggle mnemonics for items that have it explicitly flagged
    auto flagControl = QFlags<Qt::TextFlag>(flags);
    if (flagControl.testFlag(Qt::TextHideMnemonic)) {
        if (m_showMnemonics) {
            flagControl.setFlag(Qt::TextHideMnemonic, false);
            flagControl.setFlag(Qt::TextShowMnemonic);
        }
    } else if (flagControl.testFlag(Qt::TextShowMnemonic)) {
        if (!m_showMnemonics) {
            flagControl.setFlag(Qt::TextShowMnemonic, false);
            flagControl.setFlag(Qt::TextHideMnemonic);
        }
    }
    QCommonStyle::drawItemText(painter, rect, flagControl, pal, enabled, text, textRole);
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

QIcon UnionStyle::unionIcon(const Union::StylePropertyGroupPtr &properties, const QString &defaultName) const
{
    auto name = safePropertyLookup(properties, defaultName, &StylePropertyGroup::icon, &IconPropertyGroup::name);
    if (!name.isEmpty()) {
        QColor color = safePropertyLookup(properties, Union::Color{}, &StylePropertyGroup::icon, &IconPropertyGroup::color).toQColor();
        if (!color.isValid()) {
            color = standardPalette().text().color();
        }
        return Union::StyleRegistry::instance()->platform()->platformIcon(name, color);
    }
    return QIcon::fromTheme(defaultName);
}
