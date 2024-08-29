/*
    SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
    SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>

    SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Controls.impl as QQCImpl
import QtQuick.Templates as T
import org.kde.union.impl as Union

T.Button {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

    hoverEnabled: true

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
    Union.Element.hints: icon.name || icon.source.toString() ? ["with-icon"] : []

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

    Union.Positioner.positionItems: [contentItem]

    contentItem: Item {
        Union.PositionedItem.positionChildren: true

        QQCImpl.IconImage {
            Union.PositionedItem.source: Union.PositionerSource.Icon
            width: control.icon.width
            height: control.icon.height

            name: control.icon.name
            color: control.icon.color
            visible: name.length > 0
        }

        Text {
            Union.PositionedItem.source: Union.PositionerSource.Text

            text: control.text
            font: control.font
            color: Union.Style.properties.text.color

            renderType: Text.NativeRendering
        }
    }

    background: Union.StyledRectangle { }
}
