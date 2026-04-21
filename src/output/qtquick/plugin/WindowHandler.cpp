// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "WindowHandler.h"

#include <QQuickRenderControl>

#include "QuickElement.h"

using namespace Union::Quick;
using namespace Qt::StringLiterals;

class WindowInactiveHint : public ElementHint
{
public:
    WindowInactiveHint(QWindow *window, QObject *parent)
        : ElementHint(parent)
        , m_window(window)
    {
        setName(u"inactive"_s);
        m_window->installEventFilter(this);
        setWhen(!m_window->isActive());
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        // Apparently the actual window being activated/deactivated doesn't get
        // the activated/deactivated events, so instead use FocusIn/FocusOut
        // that seem to correspond to the same.
        if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut) {
            setWhen(!m_window->isActive());
        }

        return ElementHint::eventFilter(watched, event);
    }

private:
    QWindow *m_window;
};

WindowHandler::WindowHandler()
    : QObject(nullptr)
{
}

WindowHandler::~WindowHandler() = default;

void WindowHandler::initialize()
{
}

void WindowHandler::addWindow(QQuickWindow *window)
{
    if (!window) {
        return;
    }

    if (m_windows.contains(window)) {
        return;
    }

    auto renderWindow = QQuickRenderControl::renderWindowFor(window);
    if (!renderWindow) {
        renderWindow = window;
    }

    // If the window is part of a QQuickWidget, we want the parent window of the
    // widget. If the parent window isn't properly initialized yet, we end up
    // with the incorrect render window, so skip initialization and hope another
    // element initializes things at the correct time.
    if (renderWindow->inherits("QQuickWidgetOffscreenWindow")) {
        return;
    }

    WindowData data;
    data.quickWindow = window;
    data.renderWindow = renderWindow;

    // Add an "inactive" hint to the window, that can be used by styles to
    // change appearance whenever the window becomes inactive.
    //
    // TODO: This should be handled by media queries rather than a hint.
    auto element = qobject_cast<QuickElement *>(qmlAttachedPropertiesObject<QuickElement>(window));
    data.inactiveHint = new WindowInactiveHint(data.renderWindow, element);
    QQmlListReference(element, "hints").append(data.inactiveHint);

    connect(window, &QQuickWindow::destroyed, this, &WindowHandler::removeWindow);

    m_windows.insert(window, std::move(data));
}

void WindowHandler::removeWindow(QObject *window)
{
    m_windows.remove(window);
}

std::shared_ptr<WindowHandler> Union::Quick::WindowHandler::instance()
{
    static std::shared_ptr<WindowHandler> inst = std::make_shared<WindowHandler>();
    return inst;
}
