// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.ApplicationWindow {
    id: window

    Union.Element.type: "ApplicationWindow"
    Union.Element.hints: active ? [] : ["inactive"]

    color: Union.Style.properties.background.color ?? "white"
}
