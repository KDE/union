// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import org.kde.union.impl as Union

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
    ]

    hoverEnabled: Application.styleHints.useHoverEffects
    snapMode: T.Slider.SnapOnRelease

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
        Union.Element.type: "Handle"
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

    background: Union.StyledRectangle {
        x: control.leftInset + (control.horizontal ? 0 : (control.availableWidth - width) / 2)
        y: control.topInset + (control.horizontal ? (control.availableHeight - height) / 2 : 0)

        width: control.horizontal ? control.width : Union.Style.properties.layout.width
        height: control.horizontal ? Union.Style.properties.layout.height : control.height

        Union.StyledRectangle {
            Union.Element.type: "Fill"
            y: control.horizontal ? 0 : control.visualPosition * parent.height
            width: control.horizontal ? control.position * parent.width : control.implicitBackgroundWidth
            height: control.horizontal ? control.implicitBackgroundHeight : control.position * parent.height
        }

        // TODO: Creating a bunch of items just to render some small lines is not
        // very efficient. We should turn this into a more proper styled item.
        Item {
            id: tickMarks

            property int tickMarkCount: control.Union.StyleHints.tickMarkStepSize > 0 ? Math.round((control.to - control.from) / control.Union.StyleHints.tickMarkStepSize) + 1 : 0
            property real tickMarkSpacing: tickMarkCount > 1 ? (control.horizontal ? width / (tickMarkCount - 1) : height / (tickMarkCount - 1)) : 0

            x: control.horizontal ? control.handle.width / 2 : parent.width
            y: control.horizontal ? parent.height : control.handle.height / 2

            width: control.horizontal ? parent.width - control.handle.width : childrenRect.width
            height: control.horizontal ? childrenRect.height : parent.height - control.handle.height

            visible: tickMarkCount > 0

            Repeater {
                model: tickMarks.tickMarkCount

                Union.StyledRectangle {
                    id: tick

                    required property int index

                    readonly property real value: control.from + control.Union.StyleHints.tickMarkStepSize * index

                    Union.Element.type: "TickMark"
                    Union.Element.hints: Union.ElementHint { name: "active"; when: control.value >= tick.value }

                    x: control.horizontal ? Math.round(index * tickMarks.tickMarkSpacing - width / 2) : (Union.Style.properties.layout.margins.left ?? 0)
                    y: control.horizontal ? (Union.Style.properties.layout.margins.top ?? 0) : tickMarks.height - Math.round(index * tickMarks.tickMarkSpacing - height / 2)

                    width: Union.Style.properties.layout.width
                    height: Union.Style.properties.layout.height
                }
            }
        }
    }
}
