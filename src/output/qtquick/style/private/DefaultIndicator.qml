// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

Union.StyledRectangle {
    id: root

    Union.Element.subElement: "Indicator"

    Union.PositionedItem.minimumWidth: Union.Style.properties.layout.width
    Union.PositionedItem.minimumHeight: Union.Style.properties.layout.height

    implicitWidth: Union.Style.properties.layout.width ?? 0
    implicitHeight: Union.Style.properties.layout.height ?? 0

    visible: Union.Style.properties.display.visible ?? true
    opacity: Union.Style.properties.display.opacity ?? 1.0

    Union.Icon {
        // anchors.fill: parent
        anchors.centerIn: parent
        width: root.implicitWidth
        height: root.implicitHeight
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
        color: Union.Style.properties.display.color
    }
}
