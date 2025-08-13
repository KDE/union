// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.Switch {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

    Union.Element.type: "Switch"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.down
        checked: control.checked
        enabled: control.enabled
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

    icon {
        color: palette.buttonText
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }

    Union.Positioner.positionItems: [indicator, contentItem]

    indicator: Item {
        Union.Element.type: "Indicator"
        Union.Element.states {
            hovered: control.hovered
            activeFocus: control.activeFocus
            visualFocus: control.visualFocus
            pressed: control.down
            checked: control.checked
            enabled: control.enabled
        }

        implicitWidth: Union.Style.properties.layout.width ?? 0
        implicitHeight: Union.Style.properties.layout.height ?? 0

        Union.StyledRectangle { anchors.fill: parent }

        Union.StyledRectangle {
            Union.Element.type: "Handle"
            Union.Element.states {
                hovered: control.hovered
                activeFocus: control.activeFocus
                visualFocus: control.visualFocus
                pressed: control.down
                checked: control.checked
                enabled: control.enabled
            }

            anchors.verticalCenter: parent.verticalCenter

            property real minimumX: Union.Style.properties.layout.margins.left ?? 0
            property real maximumX: parent.width - width - (Union.Style.properties.layout.margins.right ?? 0)
            x: Math.max(minimumX, Math.min(maximumX, minimumX + control.visualPosition * (maximumX - minimumX)))
        }
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
            color: control.palette.text
            renderType: Text.NativeRendering
            visible: control.display !== T.AbstractButton.IconOnly
        }
    }
}
