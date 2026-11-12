// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "DragEventFilter.h"

#include <QApplication>
#include <QDialog>
#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QMouseEvent>
#include <QToolBar>
#include <QWindow>

using namespace Union::Widgets;

DragEventFilter::DragEventFilter(QObject *parent)
    : QObject(parent)
    , m_dragTimer(new QTimer(this))
    , m_clickPosition(QPointF())
    , m_currentPosition(QPointF())
    , m_currentState(DragState::Unlocked)
    , m_dragWidget(nullptr)
{
    m_dragTimer->setSingleShot(true);
    connect(m_dragTimer, &QTimer::timeout, this, &DragEventFilter::dragTimerSlot);
}

void DragEventFilter::installWidget(QWidget *widget)
{
    if (allowDrag(widget)) {
        widget->removeEventFilter(this);
        widget->installEventFilter(this);
    }
}

void DragEventFilter::uninstallWidget(QWidget *widget)
{
    if (widget) {
        widget->removeEventFilter(this);
    }
}

bool DragEventFilter::eventFilter(QObject *object, QEvent *event)
{
    auto mouseEvent = static_cast<QMouseEvent *>(event);
    if (const auto widget = qobject_cast<QWidget *>(object)) {
        switch (event->type()) {
        case QEvent::MouseButtonPress:
            return mousePressEvent(widget, mouseEvent);
        case QEvent::MouseMove:
            return mouseMoveEvent(widget, mouseEvent);
        case QEvent::MouseButtonRelease:
            return mouseReleaseEvent(widget, mouseEvent);
        default:
            break;
        }
    }
    return false;
}

bool DragEventFilter::mousePressEvent(QWidget *widget, QMouseEvent *event)
{
    // In case we are dragging mouse from ie. a toolbutton to draggable area, we are locked
    // so that the drag does not start immediately. User has to release the mouse
    // button and then click on area that can be dragged for the operation to start.
    m_currentState = DragState::Locked;
    m_dragWidget = widget;
    m_clickPosition = event->position();
    m_dragTimer->start(QApplication::startDragTime());
    return false;
}

bool DragEventFilter::mouseMoveEvent(QWidget *widget, QMouseEvent *event)
{
    // We want to start dragging when either time is cleared or the distance is cleared,
    // like in Breeze QStyle
    switch (m_currentState) {
    case DragState::Locked: {
        m_currentPosition = event->position();
        // Check if we have moved over the threshold
        if ((m_clickPosition - m_currentPosition).manhattanLength() >= QApplication::startDragDistance()) {
            m_dragWidget = widget;
            startDrag();
        }
    } break;
    case DragState::Dragging:
    case DragState::Unlocked:
        break;
    }
    return false;
}

bool DragEventFilter::mouseReleaseEvent([[maybe_unused]] QWidget *widget, [[maybe_unused]] QMouseEvent *event)
{
    m_dragTimer->stop();
    m_dragWidget = nullptr;
    m_currentState = DragState::Unlocked;
    return false;
}

bool DragEventFilter::allowDrag(QWidget *widget) const
{
    if (!widget) {
        return false;
    }
    if (m_currentState == DragState::Dragging) {
        return false;
    }

    // If we are already doing something like resizing a window, don't allow drag.
    if (widget->cursor().shape() != Qt::ArrowCursor) {
        return false;
    }

    // Accept menubars and toolbars
    if ((qobject_cast<QMenuBar *>(widget) || qobject_cast<QToolBar *>(widget))) {
        // However skip them if they're titlebars of a dockwidget
        if (auto dockWidget = qobject_cast<const QDockWidget *>(widget->parent())) {
            return widget != dockWidget->titleBarWidget();
        }
        return true;
    }
    return false;
}

void DragEventFilter::startDrag()
{
    if (allowDrag(m_dragWidget) && m_dragWidget->window() && m_dragWidget->window()->windowHandle()) {
        // We rely on startSystemMove to start and stop these drag actions properly
        m_dragWidget->window()->windowHandle()->startSystemMove();
        m_currentState = DragState::Dragging;
    }
}

void DragEventFilter::dragTimerSlot()
{
    switch (m_currentState) {
    case DragState::Locked:
        startDrag();
        break;
    case DragState::Dragging:
    case DragState::Unlocked:
        break;
    }
}
