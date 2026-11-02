// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include <QEvent>
#include <QObject>
#include <QPoint>
#include <QTimer>
#include <QWidget>

class DragEventFilter : public QObject
{
    Q_OBJECT
public:
    enum class DragState {
        Locked,
        Dragging,
        Unlocked
    };
    Q_ENUMS(DragState)

    explicit DragEventFilter(QObject *parent);

    void installWidget(QWidget *widget);
    void uninstallWidget(QWidget *widget);
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    bool mousePressEvent(QWidget *widget, QMouseEvent *event);
    bool mouseMoveEvent(QWidget *widget, QMouseEvent *event);
    bool mouseReleaseEvent(QWidget *widget, QMouseEvent *event);
    bool allowDrag(QWidget *widget) const;
    void startDrag();

    Q_SLOT void dragTimerSlot();

    QTimer *m_dragTimer;
    QPointF m_clickPosition;
    QPointF m_currentPosition;
    DragState m_currentState;
    QWidget *m_dragWidget;
};
