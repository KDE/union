// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick

import org.kde.kirigami.templates as KT

import org.kde.union as Controls
import org.kde.union.impl as Union
import org.kde.union.private as P

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
    Union.Element.hints: [
        Union.ElementHint { name: "closable"; when: control.closable },
        Union.ElementHint { name: "interactive"; when: control.interactive },
    ]
    Union.Element.attributes: P.DisplayAttribute { control: control }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding:  Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    font: Union.Style.properties.text.font.qFont

    spacing: Union.Style.properties.layout.spacing

    opacity: Union.Style.properties.display.opacity ?? 1.0

    icon {
        color: palette.buttonText
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }

    Union.Positioner.positionItems: [contentItem, indicator]

    contentItem: P.DefaultContentItem { control: control }

    indicator: Controls.ToolButton {
        Union.Element.subElement: "Indicator"

        visible: control.closable && (Union.Style.properties.display.visible ?? true)
    }

    background: Union.StyledRectangle { }
}
