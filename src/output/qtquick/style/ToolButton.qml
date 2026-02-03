// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
// SPDX-FileCopyrightText: 2024 Noah Davis <noahadvs@gmail.com>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

import "private" as P

T.ToolButton {
    id: control

    Union.Element.type: "ToolButton"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.pressed
        checked: control.checked
        enabled: control.enabled
        highlighted: control.highlighted
    }
    Union.Element.hints: [
        Union.ElementHint { name: "raised"; when: !control.flat },
        Union.ElementHint { name: "with-menu"; when: control.Accessible.role === Accessible.ButtonMenu && control.text },
    ]
    Union.Element.attributes: Union.ElementAttribute {
        name: "display"
        value: switch (control.display) {
            case T.AbstractButton.IconOnly:
                return "icon-only"
            case T.AbstractButton.TextOnly:
                return "text-only"
            case T.AbstractButton.TextBesideIcon:
                return "text-beside-icon"
            case T.AbstractButton.TextUnderIcon:
                return "text-under-icon"
        }
    }

    hoverEnabled: Application.styleHints.useHoverEffects

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

    spacing: Union.Style.properties.layout.spacing

    font: Union.Style.properties.text.font

    icon {
        color: Union.Style.properties.icon.color
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }
    flat: true

    Union.Positioner.positionItems: [contentItem, indicator]

    contentItem: P.DefaultContentItem { control: control }

    indicator: Union.Icon {
        Union.Element.type: "Indicator"
        implicitWidth: Union.Style.properties.layout.width ?? 0
        implicitHeight: Union.Style.properties.layout.height ?? 0
        name: Union.Style.properties.icon.name
        color: Union.Style.properties.icon.color
        visible: name !== ""
    }

    background: Union.StyledRectangle {}
}
