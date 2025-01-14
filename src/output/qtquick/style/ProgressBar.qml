// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
import QtQuick.Templates as T
// import QtQuick.Controls.Basic.impl

import org.kde.union.impl as Union

T.ProgressBar {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    Union.Element.type: "ProgressBar"
    Union.Element.states {
        enabled: control.enabled
    }
    Union.Element.hints: control.indeterminate ? ["indeterminate"] : []

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding:  Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    palette: Union.Style.properties.palette.quickPalette

    contentItem: Item {
        id: trackContainer

        Union.StyledRectangle {
            id: track
            Union.Element.type: "Track"

            property real indeterminatePosition: 0

            x: control.indeterminate ? indeterminatePosition * (parent.width - width) : 0
            // TODO: Find a way to style the indeterminate width here
            width: control.indeterminate ? 0.25 * parent.width : control.visualPosition * parent.width
            height: parent.height

            // TODO: Some way of controlling what kind of animation to use from
            // the style.
            SequentialAnimation on indeterminatePosition {
                loops: Animation.Infinite
                running: control.indeterminate && control.contentItem.visible

                NumberAnimation {
                    duration: 1000
                    easing.type: Easing.InOutSine
                    to: 1
                }
                NumberAnimation {
                    duration: 1000
                    easing.type: Easing.InOutSine
                    to: 0
                }

            }
        }
    }

    background: Union.StyledRectangle { }
}
