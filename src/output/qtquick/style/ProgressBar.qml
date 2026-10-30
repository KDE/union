// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

import "private" as P

T.ProgressBar {
    id: control

    Union.Element.type: "ProgressBar"
    Union.Element.states {
        enabled: control.enabled
    }
    Union.Element.hints: [
        Union.ElementHint { name: "horizontal" },
        Union.ElementHint { name: "indeterminate"; when: control.indeterminate }
    ]

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    hoverEnabled: false

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding:  Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    contentItem: null

    background: P.BarBackground {
        id: background
        control: control
        horizontal: true
        from: control.from
        to: control.to
        startPosition: control.indeterminate ? indeterminatePosition : 0
        endPosition: control.indeterminate ? -1 : control.position
        tickMarkStepSize: control.Union.StyleHints.tickMarkStepSize

        // TODO: Some way of controlling what kind of animation to use from
        // the style.
        property real indeterminatePosition: 0
        SequentialAnimation on indeterminatePosition {
            loops: Animation.Infinite
            running: control.indeterminate && control.visible

            NumberAnimation {
                duration: 1000
                easing.type: Easing.InOutSine
                to: 1.0
            }
            NumberAnimation {
                duration: 1000
                easing.type: Easing.InOutSine
                to: 0
            }
        }
    }
}
