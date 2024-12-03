// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
import QtQuick.Controls.impl as QQCImpl
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.MenuItem {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

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
    palette: Union.Style.properties.palette.quickPalette

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

        // This is wrapped in an Item because IconImage's implicit size is only
        // settable through sourceSize but that is ignored if there is no source
        // which means the icon cannot reserve size even if it should because
        // the menu has icons.
        Item {
            Union.PositionedItem.source: Union.PositionerSource.Icon

            implicitWidth:  control.icon.width
            implicitHeight:  control.icon.height

            visible: control.ListView.view?.hasIcons ?? false

            QQCImpl.IconImage {
                anchors.fill: parent

                name: control.icon.name
                color: control.icon.color

                sourceSize.width: control.icon.width
                sourceSize.height: control.icon.height
            }
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

    arrow: QQCImpl.IconImage {
        Union.Element.type: "Arrow"

        visible: control.subMenu
        name: Union.Style.properties.icon.name ?? ""

        sourceSize.width: Union.Style.properties.layout.width ?? 0
        sourceSize.height: Union.Style.properties.layout.height ?? 0
    }

    background: Union.StyledRectangle { }
}
