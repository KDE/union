// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.RadioButton {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

    Union.Element.type: "RadioButton"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.pressed
        checked: control.checked
        enabled: control.enabled
    }
    Union.Element.hints: icon.name || icon.source.toString() ? ["with-icon"] : []
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
    bottomPadding: Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    spacing: Union.Style.properties.layout.spacing

    font: Union.Style.properties.text.font

    icon {
        color: Union.Style.properties.icon.color
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }

    Union.Positioner.positionItems: [contentItem, indicator]

    indicator: Union.StyledRectangle {
        Union.Element.type: "Indicator"
    }

    contentItem: Item {
        Union.PositionedItem.positionChildren: true
        Union.Icon {
            Union.PositionedItem.source: Union.PositionerSource.Icon
            control: control
            visible: name && control.display !== T.AbstractButton.TextOnly
        }
        Text {
            Union.PositionedItem.source: Union.PositionerSource.Text
            text: control.text
            font: control.font
            color: control.palette.windowText
            renderType: Text.NativeRendering
            visible: control.display !== T.AbstractButton.IconOnly && text.length > 0
        }
    }

    background: Union.StyledRectangle { }
}
