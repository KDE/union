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
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

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

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

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

    contentItem: QCCImpl.IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display
        icon: control.icon
        text: control.text
        font: control.font
        color: control.palette.buttonText
        alignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal) | Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)
    }

    background: Union.StyledRectangle {}
}
