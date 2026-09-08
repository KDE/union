// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

Item {
    id: root

    required property T.AbstractButton control

    Union.PositionedItem.positionChildren: true
    Union.Mnemonics.enabled: root.control.enabled && root.control.visible
    Union.Mnemonics.controlType: Union.Mnemonics.ActionElement
    Union.Mnemonics.label: root.control.text

    readonly property real iconWidth: iconItem.Union.Style.properties.layout.width
    readonly property real iconHeight: iconItem.Union.Style.properties.layout.width
    readonly property real iconColor: iconItem.Union.Style.properties.display.color
    readonly property real iconName: iconItem.Union.Style.properties.icon.name
    readonly property real iconSource: iconItem.Union.Style.properties.icon.source

    Union.Icon {
        id: iconItem

        Union.Element.subElement: "Icon"

        Union.PositionedItem.minimumWidth: Union.Style.properties.layout.width ?? 0.0
        Union.PositionedItem.minimumHeight: Union.Style.properties.layout.height ?? 0.0

        control: root.control
        visible: (name || source.toString()) && (Union.Style.properties.display.visible ?? true)
        opacity: Union.Style.properties.display.opacity ?? 1.0
    }

    Text {
        Union.Element.subElement: "Text"

        text: root.Union.Mnemonics.richTextLabel ?? root.control.text
        font: root.control.font
        color: Union.Style.properties.display.color ?? "black"
        elide: wrapMode == Text.NoWrap && Union.Style.properties.text.elide ? Union.Style.properties.text.elide : Text.ElideNone
        wrapMode: Union.Style.properties.text.wrapMode ?? Text.NoWrap

        visible: text.length > 0 && (Union.Style.properties.display.visible ?? true)
        opacity: Union.Style.properties.display.opacity ?? 1.0

        horizontalAlignment: Union.Style.properties.text.alignment ?? Qt.AlignLeft
    }
}
