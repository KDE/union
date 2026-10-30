// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.GroupBox {
    id: control

    Union.Element.type: "GroupBox"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        enabled: control.enabled
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding,
                            implicitLabelWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top + implicitLabelHeight
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    spacing: Union.Style.properties.layout.spacing

    label: Item {
        Union.Element.subElement: "Header"

        x: Union.Style.properties.layout.margins.left
        y: Union.Style.properties.layout.margins.top
        width: control.width - (Union.Style.properties.layout.margins.left ?? 0) - (Union.Style.properties.layout.margins.right ?? 0)

        implicitWidth: Math.max(Union.Style.properties.layout.width ?? 0, title.implicitWidth)
                       + (Union.Style.properties.layout.padding.left ?? 0) + (Union.Style.properties.layout.padding.right ?? 0)
        implicitHeight: Math.max(Union.Style.properties.layout.height ?? 0, title.implicitHeight)
                        + (Union.Style.properties.layout.padding.top ?? 0) + (Union.Style.properties.layout.padding.bottom ?? 0)

        Union.StyledRectangle {
            anchors.fill: parent
        }

        Text {
            id: title

            anchors {
                fill: parent
                leftMargin: Union.Style.properties.layout.padding.left
                rightMargin: Union.Style.properties.layout.padding.right
                topMargin: Union.Style.properties.layout.padding.top
                bottomMargin: Union.Style.properties.layout.padding.bottom
            }

            text: control.title
            font: Union.Style.properties.text.font.qFont ?? Application.font
            color: Union.Style.properties.display.color ?? control.palette.text
            wrapMode: Union.Style.properties.text.wrapMode ?? Text.NoWrap
            elide: wrapMode == Text.NoWrap ? (Union.Style.properties.text.elide ?? Text.ElideRight) : Text.ElideNone

            horizontalAlignment: Union.Style.properties.text.alignment ?? Qt.AlignLeft
        }
    }

    background: Union.StyledRectangle { }
}
