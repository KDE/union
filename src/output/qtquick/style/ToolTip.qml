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
    // Make sure the tooltip doesn't overlap with the cursor, otherwise we end
    // up in a loop with tooltips that show on hovered.
    y: -implicitHeight + bottomInset

    implicitWidth: Math.max(implicitBackgroundWidth + (Union.Style.properties.layout.inset.left ?? 0) + (Union.Style.properties.layout.inset.right ?? 0),
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + (Union.Style.properties.layout.inset.top ?? 0) + (Union.Style.properties.layout.inset.bottom ?? 0),
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: -(Union.Style.properties.layout.margins.left ?? 0)
    rightInset: -(Union.Style.properties.layout.margins.right ?? 0)
    topInset: -(Union.Style.properties.layout.margins.top ?? 0)
    bottomInset: -(Union.Style.properties.layout.margins.bottom ?? 0)

    font: Union.Style.properties.text.font

    closePolicy: T.Popup.CloseOnEscape | T.Popup.CloseOnPressOutsideParent | T.Popup.CloseOnReleaseOutsideParent
    delay: 700
    // Never time out while being hovered; it's annoying
    timeout: -1

    popupType: T.Popup.Item

    contentItem: Text {
        text: control.text
        font: control.font
        wrapMode: Text.Wrap
        color: Union.Style.properties.text.color ?? control.palette.text
        horizontalAlignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal)
        verticalAlignment: Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)
    }

    background: Item {
        // See explanation in Menu.qml, this uses the same trick to get a larger
        // window.
        implicitWidth: Union.Style.properties.layout.width ?? 0
        implicitHeight: Union.Style.properties.layout.height ?? 0

        Union.StyledRectangle {
            anchors {
                fill: parent
                leftMargin: (Union.Style.properties.layout.margins.left ?? 0) + (Union.Style.properties.layout.inset.left ?? 0)
                rightMargin: (Union.Style.properties.layout.margins.right ?? 0) + (Union.Style.properties.layout.inset.right ?? 0)
                topMargin: (Union.Style.properties.layout.margins.top ?? 0) + (Union.Style.properties.layout.inset.top ?? 0)
                bottomMargin: (Union.Style.properties.layout.margins.bottom ?? 0) + (Union.Style.properties.layout.inset.bottom ?? 0)
            }
        }
    }
}
