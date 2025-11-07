// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.ToolTip {
    id: control

    Union.Element.type: "ToolTip"
    Union.Element.states {
        activeFocus: control.activeFocus
        enabled: control.enabled
    }

    x: parent ? (parent.width - implicitWidth) / 2 : 0
    y: -implicitHeight

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    leftMargin: Union.Style.properties.layout.margins.left
    rightMargin: Union.Style.properties.layout.margins.right
    topMargin: Union.Style.properties.layout.margins.top
    bottomMargin: Union.Style.properties.layout.margins.bottom

    font: Union.Style.properties.text.font

    closePolicy: T.Popup.CloseOnEscape | T.Popup.CloseOnPressOutsideParent | T.Popup.CloseOnReleaseOutsideParent
    delay: 700
    // Never time out while being hovered; it's annoying
    timeout: -1

    contentItem: Text {
        text: control.text
        font: control.font
        wrapMode: Text.Wrap
        color: Union.Style.properties.text.color ?? control.palette.text
        horizontalAlignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal)
        verticalAlignment: Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)
    }

    background: Union.StyledRectangle {}
}
