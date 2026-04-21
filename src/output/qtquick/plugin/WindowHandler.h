// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <qqmlregistration.h>

#include "unionquickimpl_export.h"

class QWindow;
class QQuickWindow;

namespace Union
{
namespace Quick
{
class ElementHint;

class UNIONQUICKIMPL_EXPORT WindowHandler : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    WindowHandler();
    ~WindowHandler() override;

    void initialize();

    /*!
     * Add a window to handle.
     */
    void addWindow(QQuickWindow *window);
    void removeWindow(QObject *window);

    /*!
     * A version of Window.startSystemMove() that handles non-QQuickWindows.
     *
     * QML Window.window may return null if the item it's attached to is in a
     * QQuickWidget or similar. To get the actual window, you need to call
     * QQuickRenderControl::renderWindowFor() but this cannot be done from QML.
     * So this function will start a system move on the render window of the
     * given item.
     */
    Q_INVOKABLE void startSystemMove(QQuickItem *item);

    static std::shared_ptr<WindowHandler> instance();

    static WindowHandler *create(QQmlEngine *engine, QJSEngine *)
    {
        auto inst = instance();

        Q_ASSERT(engine->thread() == inst->thread());

        QJSEngine::setObjectOwnership(inst.get(), QJSEngine::CppOwnership);
        return inst.get();
    }

private:
    struct WindowData {
        QQuickWindow *quickWindow = nullptr;
        QWindow *renderWindow = nullptr;
        ElementHint *inactiveHint = nullptr;
    };

    QHash<QObject *, WindowData> m_windows;
};

}
}
