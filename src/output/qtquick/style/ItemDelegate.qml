// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
// SPDX-FileCopyrightText: 2024 Noah Davis <noahadvs@gmail.com>

import QtQuick
import QtQuick.Controls.impl as QQCImpl
import QtQuick.Templates as T
import org.kde.union.impl as Union

T.ItemDelegate {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

    hoverEnabled: Application.styleHints.useHoverEffects

    Union.Element.type: "ItemDelegate"
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
    Union.Element.attributes: {
        let result = {}
        switch (display) {
        case T.AbstractButton.IconOnly:
            result.display = "icon-only"
            break
        case T.AbstractButton.TextOnly:
            result.display = "text-only"
            break
        case T.AbstractButton.TextBesideIcon:
            result.display = "text-beside-icon"
            break
        case T.AbstractButton.TextUnderIcon:
            result.display = "text-under-icon"
            break
        }
        return result
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

    font: Union.Style.properties.text.font

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
            sourceSize.width: control.icon.width
            sourceSize.height: control.icon.height
            name: control.icon.name
            source: control.icon.source
            color: control.icon.color
            visible: control.display !== T.AbstractButton.TextOnly
        }
        Text {
            Union.PositionedItem.source: Union.PositionerSource.Text
            text: control.text
            font: control.font
            color: Union.Style.properties.text.color ?? "black"
            horizontalAlignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal)
            verticalAlignment: Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)
            renderType: Text.NativeRendering
            visible: control.display !== T.AbstractButton.IconOnly
        }
    }

    background: Union.StyledRectangle {}
}
