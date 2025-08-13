// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Noah Davis <noahadvs@gmail.com>

import QtQuick
import QtQuick.Controls.impl as QCCImpl
import QtQuick.Templates as T
import org.kde.union.impl as Union

T.TabButton {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

    hoverEnabled: Application.styleHints.useHoverEffects

    Union.Element.type: "TabButton"
    Union.Element.colorSet: Union.ColorSet.Button
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.pressed
        checked: control.checked
        enabled: control.enabled
    }
    Union.Element.hints: !control.ListView?.view?.highlightItem ? [] : ["itemview-highlight"]
    Union.Element.attributes: control.T.TabBar.position === T.TabBar.Footer ?
        {"tab-position": "south"} : {"tab-position": "north"}

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding: Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    font: Union.Style.properties.text.font

    spacing: 6
    icon {
        color: Union.Style.properties.icon.color
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }

    Union.Positioner.positionItems: ["contentItem"]

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

    background: Union.StyledRectangle {}
}
