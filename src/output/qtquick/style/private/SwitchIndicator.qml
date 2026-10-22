// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

Union.StyledRectangle {
    id: root

    required property T.Control control

    Union.Element.subElement: "Track"

    Union.PositionedItem.minimumWidth: implicitWidth
    Union.PositionedItem.minimumHeight: implicitHeight

    Union.StyledRectangle {
        id: handle
        Union.Element.subElement: "Handle"

        anchors.verticalCenter: parent.verticalCenter

        property real minimumX: Union.Style.properties.layout.margins.left ?? 0
        property real maximumX: parent.width - width - (Union.Style.properties.layout.margins.right ?? 0)
        x: Math.max(minimumX, Math.min(maximumX, minimumX + root.control.visualPosition * (maximumX - minimumX)))
    }
}
