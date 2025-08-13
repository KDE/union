/* SPDX-FileCopyrightText: 2025 Niccolò Venerandi <niccolo@venerandi.com>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.RadioDelegate {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

    Union.Element.type: "RadioDelegate"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.pressed
        checked: control.checked
        enabled: control.enabled
        highlighted: control.highlighted
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

    Union.Positioner.positionItems: [contentItem, indicator]

    icon {
        color: palette.windowText
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
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

    indicator: Union.StyledRectangle {
        Union.Element.type: "Indicator"
    }

    background: Union.StyledRectangle { }
}
