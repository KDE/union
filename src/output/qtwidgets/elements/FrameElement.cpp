// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "FrameElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QAbstractScrollArea>
#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QDockWidget>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>
#include <QStackedLayout>
#include <QStyle>
#include <QTextBrowser>

using namespace Qt::StringLiterals;
using namespace Union::Properties;

FrameElement::FrameElement(const QStyleOptionFrame *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_frameOption(option)
{
    update();
}

FrameElement::~FrameElement()
{
}

void FrameElement::update()
{
    updateSubElementList();
    layout();
}

void FrameElement::layout()
{
    // Backwards compatibility for borders, as applications used this custom Breeze property to declare
    // which borders are applied to the widget.

    // Background and content is separate
    m_backgroundElementList = prepareElements(m_styleOption, m_widget);
    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
        m_layoutMap = layoutMap(m_backgroundElementList, m_styleOption, m_subElementList);
    }
    if (m_backgroundProperties) {
        m_isValid = true;
    } else {
        m_isValid = false;
    }
}

void FrameElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }
    // Application has requested to disable frames for this element
    if (m_widget) {
        const auto forceFrame = m_widget->property("_breeze_force_frame");
        if (forceFrame.isValid() && !forceFrame.toBool()) {
            return;
        }
    }

    switch (enums.ControlElement) {
    case QStyle::CE_FocusFrame:
    case QStyle::CE_ShapedFrame:
        // Skip drawing frames for items that have framewidth of 0, draw only background
        if (pixelMetric(QStyle::PM_DefaultFrameWidth) == 0) {
            drawPanel(painter);
        } else {
            drawBackground(painter);
        }
        break;
    }
    // We should be prepared to draw any potential frame element
    // even if not explicitly casted to do so.
    switch (enums.PrimitiveElement) {
    case QStyle::PE_Frame:
    case QStyle::PE_FrameDefaultButton:
    case QStyle::PE_FrameDockWidget:
    case QStyle::PE_FrameFocusRect:
    case QStyle::PE_FrameGroupBox:
    case QStyle::PE_FrameLineEdit:
    case QStyle::PE_FrameMenu:
    case QStyle::PE_FrameStatusBarItem:
    case QStyle::PE_FrameTabWidget:
    case QStyle::PE_FrameWindow:
    case QStyle::PE_FrameButtonBevel:
    case QStyle::PE_FrameButtonTool:
    case QStyle::PE_FrameTabBarBase:
        drawFrame(painter);
        break;
    }
}

void FrameElement::drawFrame(QPainter *painter) const
{
    // Follow the expectations: If frame has panel shape,
    // we need to draw the background too. In HLine/VLine cases
    // we can just skip it
    if (!m_frameOption) {
        return;
    }

    switch (m_frameOption->frameShape) {
    case QFrame::NoFrame:
        // Draw nothing
        return;
    case QFrame::Box:
    case QFrame::Panel:
    case QFrame::WinPanel:
    case QFrame::StyledPanel:
        drawBackground(painter);
        break;
    case QFrame::HLine:
    case QFrame::VLine:
        AbstractElement::drawBackground(painter);
        break;
    }
}

void FrameElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(ElementString::Frame);
}

QVariantMap FrameElement::elementAttributes() const
{
    QVariantMap map;
    if (!m_frameOption) {
        return map;
    }
    switch (m_frameOption->frameShape) {
    case QFrame::NoFrame:
        map[u"shape"_s] = u"no-frame"_s;
        break;
    case QFrame::Box:
        map[u"shape"_s] = u"box"_s;
        break;
    case QFrame::Panel:
    case QFrame::WinPanel:
    case QFrame::StyledPanel:
        map[u"shape"_s] = u"panel"_s;
        break;
    case QFrame::HLine:
        map[u"shape"_s] = u"horizontal-line"_s;
        break;
    case QFrame::VLine:
        map[u"shape"_s] = u"vertical-line"_s;
        break;
    default:
        break;
    }

    return map;
}

QStringList FrameElement::elementHints() const
{
    auto hints = frameHints(m_frameOption);

    // Backwards compatibility:
    // Custom KDE style hint used by KDE widgets applications, for declaring sidebars
    if (m_widget) {
        if (m_widget->inherits("KDEPrivate::KPageListView") || m_widget->inherits("KDEPrivate::KPageTreeView")) {
            const bool reverseLayout(m_styleOption && m_styleOption->direction == Qt::RightToLeft);
            if (reverseLayout) {
                hints.append(u"panel-right"_s);
            } else {
                hints.append(u"panel-left"_s);
            }
        }
    }

    if (m_widget && m_widget->parentWidget()) {
        if (const auto dock = qobject_cast<const QDockWidget *>(m_widget->parentWidget())) {
            // For some reason the dock->dockLocation segfaults if there are no dockWidgetFeatures set
            // so check for that.
            if (!dock->features().testFlag(QDockWidget::NoDockWidgetFeatures)) {
                switch (dock->dockLocation()) {
                case Qt::LeftDockWidgetArea:
                    hints.append(u"dock-left"_s);
                    break;
                case Qt::RightDockWidgetArea:
                    hints.append(u"dock-right"_s);
                    break;
                case Qt::TopDockWidgetArea:
                    hints.append(u"dock-top"_s);
                    break;
                case Qt::BottomDockWidgetArea:
                    hints.append(u"dock-bottom"_s);
                    break;
                case Qt::DockWidgetArea_Mask:
                case Qt::NoDockWidgetArea:
                    break;
                }
            }
        }
    }
    return hints;
}

QLayout *findParentLayout(const QWidget *widget)
{
    if (!widget->parentWidget()) {
        return nullptr;
    }

    auto layout = widget->parentWidget()->layout();
    if (!layout) {
        return nullptr;
    }

    if (layout->indexOf(const_cast<QWidget *>(widget)) > -1) {
        return layout;
    }

    QList<QObject *> children = layout->children();

    while (!children.isEmpty()) {
        layout = qobject_cast<QLayout *>(children.takeFirst());
        if (!layout) {
            continue;
        }

        if (layout->indexOf(const_cast<QWidget *>(widget)) > -1) {
            return layout;
        }
        children += layout->children();
    }

    return nullptr;
}

qreal FrameElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    // Use same heuristics for drawing frames as Breeze does.
    // TODO: We may want to expose this to the user somehow.
    if (pixelMetric == QStyle::PM_DefaultFrameWidth) {
        if (!m_widget) {
            return 0;
        }
        const auto frameWidth = averageBorderSize();

        if (qobject_cast<const QMenu *>(m_widget)) {
            return frameWidth;
        } else if (qobject_cast<const QLineEdit *>(m_widget) || qobject_cast<const QLineEdit *>(m_widget->parentWidget())) {
            return frameWidth;
        } else if (qobject_cast<const QComboBox *>(m_widget) || qobject_cast<const QComboBox *>(m_widget->parentWidget())) {
            return frameWidth;
        }

        const auto forceFrame = m_widget->property("_breeze_force_frame");
        if (forceFrame.isValid() && !forceFrame.toBool()) {
            return 0;
        }
        if ((forceFrame.isValid() && forceFrame.toBool()) || m_widget->property("_breeze_borders_sides").isValid()) {
            return frameWidth;
        }

        if (qobject_cast<const QAbstractScrollArea *>(m_widget)) {
            auto layout = findParentLayout(m_widget);

            if (!layout) {
                if (m_widget->parentWidget() && m_widget->parentWidget()->layout()) {
                    layout = m_widget->parentWidget()->layout();
                }
            }

            if (layout) {
                if (layout->inherits("QDockWidgetLayout") || layout->inherits("QMainWindowLayout") || qobject_cast<const QStackedLayout *>(layout)) {
                    return 0;
                }

                if (auto grid = qobject_cast<const QGridLayout *>(layout)) {
                    if (grid->horizontalSpacing() > 0 || grid->verticalSpacing() > 0) {
                        return frameWidth;
                    }
                }

                // Add frame when scroll area is in a layout with more than an item and the
                // layout has some spacing.
                if (layout->spacing() > 0 && layout->count() > 1) {
                    return frameWidth;
                }
            }
        }

        if (qobject_cast<const QTabWidget *>(m_widget) || qobject_cast<const QTextBrowser *>(m_widget)) {
            return frameWidth;
        }

        // fallback
        return 0;
    }
    return 0;
}

QRectF FrameElement::subElementRect(QStyle::SubElement element) const
{
    return m_style->QCommonStyle::subElementRect(element, m_styleOption, m_widget);
}
