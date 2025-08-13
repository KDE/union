// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick

import QtQuick.Controls.impl as QQCImpl

import org.kde.kirigami.templates as KT

import org.kde.union as Union
import org.kde.union.impl as Impl

KT.Chip {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Impl.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Impl.Positioner.implicitHeight)

    Impl.Element.type: "Chip"
    Impl.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.down
        checked: control.checked
        enabled: control.enabled
    }

    Impl.Element.hints: {
        let result = []
        if (control.closable) {
            result.push("closable")
        }
        if (control.interactive) {
            result.push("interactive")
        }
        return result
    }

    Impl.Element.attributes: {
        let result = {}
        switch (control.display) {
        case KT.Chip.IconOnly:
            result.display = "icon-only"
            break
        case KT.Chip.TextOnly:
            result.display = "text-only"
            break
        case KT.Chip.TextBesideIcon:
            result.display = "text-beside-icon"
            break
        case KT.Chip.TextUnderIcon:
            result.display = "text-under-icon"
            break
        }
        return result
    }

    leftPadding: Impl.Positioner.padding.left
    rightPadding: Impl.Positioner.padding.right
    topPadding: Impl.Positioner.padding.top
    bottomPadding:  Impl.Positioner.padding.bottom

    leftInset: Impl.Style.properties.layout.inset.left
    rightInset: Impl.Style.properties.layout.inset.right
    topInset: Impl.Style.properties.layout.inset.top
    bottomInset: Impl.Style.properties.layout.inset.bottom

    font: Impl.Style.properties.text.font

    spacing: Impl.Style.properties.layout.spacing

    icon {
        color: palette.buttonText
        width: Impl.Style.properties.icon.width
        height: Impl.Style.properties.icon.height
        name: Impl.Style.properties.icon.name
        source: Impl.Style.properties.icon.source
    }

    Impl.Positioner.positionItems: [contentItem, indicator]

    contentItem: Item {
        Impl.PositionedItem.positionChildren: true

        QQCImpl.IconImage {
            Impl.PositionedItem.source: Impl.PositionerSource.Icon

            name: control.icon.name
            color: control.icon.color
            visible: name.length > 0 && control.display != KT.Chip.TextOnly

            sourceSize.width: control.icon.width
            sourceSize.height: control.icon.height
        }

        Text {
            Impl.PositionedItem.source: Impl.PositionerSource.Text

            text: control.text
            font: control.font
            color: control.palette.buttonText

            visible: control.display != KT.Chip.IconOnly
        }
    }

    indicator: Union.ToolButton {
        id: indicator

        Union.Element.hints: ["indicator"]

        visible: control.closable
    }

    background: Impl.StyledRectangle { }
}
