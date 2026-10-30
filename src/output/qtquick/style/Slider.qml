// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import org.kde.union.impl as Union

import "private" as P

T.Slider {
    id: control

    Union.Element.type: "Slider"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.pressed
        enabled: control.enabled
    }
    Union.Element.hints: [
        Union.ElementHint { name: "horizontal"; when: control.horizontal },
        Union.ElementHint { name: "vertical"; when: !control.horizontal },
        Union.ElementHint { name: "changed"; when: false },
    ]

    hoverEnabled: Application.styleHints.useHoverEffects
    snapMode: T.Slider.SnapOnRelease
    wheelEnabled: true

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitHandleWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitHandleHeight + topPadding + bottomPadding)

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    handle: Union.StyledRectangle {
        Union.Element.subElement: "Handle"
        Union.Element.states {
            hovered: control.hovered
            activeFocus: control.handle.activeFocus
            visualFocus: control.visualFocus
            pressed: control.pressed
            enabled: control.handle.enabled
        }
        x: control.leftPadding + (control.horizontal ? control.visualPosition * (control.availableWidth - width) : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : control.visualPosition * (control.availableHeight - height))
    }

    background: P.BarBackground {
        id: background

        control: control
        horizontal: control.horizontal
        from: control.from
        to: control.to
        startPosition: 0
        endPosition: control.position
        tickMarkStepSize: control.Union.StyleHints.tickMarkStepSize
        handleWidth: control.handle.width
        handleHeight: control.handle.height
    }
}
