// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.MenuItem {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: {
        if (visible) {
            return Math.max(implicitBackgroundHeight + topInset + bottomInset, Union.Positioner.implicitHeight)
        } else {
            return 0
        }
    }

    Union.Element.type: "MenuItem"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.down
        checked: control.checked
        enabled: control.enabled
        highlighted: control.highlighted
    }

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding:  Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    spacing: Union.Style.properties.layout.spacing

    font: Union.Style.properties.text.font

    icon {
        color: palette.windowText
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }

    Union.Positioner.positionItems: [contentItem, indicator, arrow]

    contentItem: Item {
        Union.PositionedItem.positionChildren: true

        Union.Icon {
            Union.PositionedItem.source: Union.PositionerSource.Icon
            control: control
            visible: control.ListView.view?.hasIcons ?? false
        }

        Text {
            Union.PositionedItem.source: Union.PositionerSource.Text

            text: control.text
            font: control.font
            color: control.palette.buttonText
        }
    }

    indicator: Item {
        Union.Element.type: "Indicator"

        implicitWidth: Union.Style.properties.layout.width ?? 0
        implicitHeight: Union.Style.properties.layout.height ?? 0
        visible: control.checkable || (control.ListView.view?.hasCheckable ?? false)

        Union.StyledRectangle {
            anchors.fill: parent
            visible: control.checkable
        }
    }

    arrow: Union.Icon {
        Union.Element.type: "Arrow"

        implicitWidth: Union.Style.properties.layout.width
        implicitHeight: Union.Style.properties.layout.height

        name: Union.Style.properties.icon.name
        color: Union.Style.properties.icon.color
        visible: name && control.subMenu
    }

    background: Union.StyledRectangle { }
}
