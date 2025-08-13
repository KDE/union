// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QEvent>

/*!
 * A helper template that helps with registering event types for custom events.
 *
 * Use it by declaring a static instance of this struct with your custom event
 * types as template arguments. It is assumed the event class has a public
 * static member s_type for the event type.
 */
template<typename... Args>
struct EventTypeRegistration {
    EventTypeRegistration()
    {
        ((Args::s_type = QEvent::Type(QEvent::registerEventType())), ...);
    }
};
