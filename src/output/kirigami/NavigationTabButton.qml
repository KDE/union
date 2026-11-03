/* SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 * SPDX-FileCopyrightText: 2021 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T

import org.kde.kirigami.templates as KT

import org.kde.union.impl as Union
import org.kde.union.private as P

KT.NavigationTabButton {
    id: control

    Union.Element.type: "NavigationTabButton"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.pressed
        checked: control.checked
        enabled: control.enabled
    }

    Union.Element.attributes: [
        Union.ElementAttribute {
            name: "direction"
            value: control.T.TabBar.position === T.TabBar.Footer ? "bottom" : "top"
        },
        Union.ElementAttribute {
            property T.AbstractButton control
            name: "display"
            value: switch (control.display) {
            case T.AbstractButton.IconOnly:
                return "icon-only";
            case T.AbstractButton.TextOnly:
                return "text-only";
            case T.AbstractButton.TextBesideIcon:
                return "text-beside-icon";
            case T.AbstractButton.TextUnderIcon:
                return "text-under-icon";
            }
        }
    ]

    Union.Positioner.positionItems: [contentItem, indicator]

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             Union.Positioner.implicitHeight)

    hoverEnabled: Application.styleHints.useHoverEffects

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    font: Union.Style.properties.text.font.qFont

    spacing: Union.Style.properties.layout.spacing

    opacity: Union.Style.properties.display.opacity

    icon {
        color: Union.Style.properties.icon.color
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }

    display: T.AbstractButton.TextUnderIcon

    contentItem: P.DefaultContentItem { control: control }

    indicator: P.DefaultIndicator { }

    background: Union.StyledRectangle {}
}
