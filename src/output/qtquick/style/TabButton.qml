// SPDX-FileCopyrightText: 2024 Noah Davis <noahadvs@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-or-later

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

    leftPadding: Union.Style.padding.left
    rightPadding: Union.Style.padding.right
    topPadding: Union.Style.padding.top
    bottomPadding: Union.Style.padding.bottom

    leftInset: Union.Style.margins.left
    rightInset: Union.Style.margins.right
    topInset: Union.Style.margins.top
    bottomInset: Union.Style.margins.bottom

    font: Union.Style.text.font

    spacing: 6
    icon {
        color: Union.Style.icon.color
        width: Union.Style.icon.width
        height: Union.Style.icon.height
        name: Union.Style.icon.name
        source: Union.Style.icon.source
    }

    contentItem: QCCImpl.IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display
        icon: control.icon
        text: control.text
        font: control.font
        color: control.palette.buttonText
        alignment: Union.Style.text.horizontalAlignment | Union.Style.text.verticalAlignment
    }

    background: Union.StyledRectangle {}
}
