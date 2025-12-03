// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.Button {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

    Union.Element.type: "Button"
    Union.Element.colorSet: Union.ColorSet.Button
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.down
        checked: control.checked
        enabled: control.enabled
        highlighted: control.highlighted
    }
    Union.Element.hints: {
        let result = []
        if (icon.name || icon.source.toString()) {
            result.push("with-icon")
        }
        if (flat) {
            result.push("flat")
        }
        // Match qqc2-desktop-style's logic.
        // TODO: Figure out how much sense this actually makes, I don't like the
        // text condition here.
        if (Accessible.role === Accessible.ButtonMenu && text) {
            result.push("with-menu")
        }
        return result
    }
    Union.Element.attributes: {
        let result = {}
        switch (display) {
        case T.AbstractButton.IconOnly:
            result.display = "icon-only"
            break
        case T.AbstractButton.TextOnly:
            result.display = "text-only"
            break
        case T.AbstractButton.TextBesideIcon:
            result.display = "text-beside-icon"
            break
        case T.AbstractButton.TextUnderIcon:
            result.display = "text-under-icon"
            break
        }
        return result
    }

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding:  Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    font: Union.Style.properties.text.font

    spacing: Union.Style.properties.layout.spacing

    icon {
        color: Union.Style.properties.icon.color
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }

    Union.Positioner.positionItems: [contentItem, indicator]

    contentItem: Item {
        Union.PositionedItem.positionChildren: true

        Union.Icon {
            Union.PositionedItem.source: Union.PositionerSource.Icon
            control: control
            visible: name && control.display != T.AbstractButton.TextOnly
        }

        Text {
            Union.PositionedItem.source: Union.PositionerSource.Text

            text: control.text
            font: control.font
            color: Union.Style.properties.text.color

            visible: control.display != T.AbstractButton.IconOnly
        }
    }

    indicator: Union.Icon {
        Union.Element.type: "Indicator"
        implicitWidth: Union.Style.properties.layout.width ?? 0
        implicitHeight: Union.Style.properties.layout.height ?? 0
        name: Union.Style.properties.icon.name
        color: Union.Style.properties.icon.color
        visible: name !== ""
    }

    background: Union.StyledRectangle { }
}
