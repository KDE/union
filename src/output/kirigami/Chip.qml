// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick

import org.kde.kirigami.templates as KT

import org.kde.union as Controls
import org.kde.union.impl as Union

KT.Chip {
    id: control

    Union.Element.type: "Chip"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.down
        checked: control.checked
        enabled: control.enabled
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

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
        color: palette.buttonText
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

            width: control.icon.width
            height: control.icon.height

            name: control.icon.name
            color: control.icon.color
            visible: name.length > 0 && control.display != KT.Chip.TextOnly
        }

        Text {
            Union.PositionedItem.source: Union.PositionerSource.Text

            text: control.text
            font: control.font
            color: control.palette.buttonText

            visible: control.display != KT.Chip.IconOnly
        }
    }

    indicator: Controls.ToolButton {
        id: indicator

        Union.Element.hints: [
            Union.ElementHint { name: "indicator" }
        ]

        visible: control.closable
    }

    background: Union.StyledRectangle { }
}
