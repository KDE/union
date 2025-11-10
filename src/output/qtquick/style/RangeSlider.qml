// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.RangeSlider {
    id: control
    Union.Element.type: "RangeSlider"
    Union.Element.hints: control.horizontal ? ["horizontal"] : ["vertical"]
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        enabled: control.enabled
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, first.implicitHandleWidth + leftPadding + rightPadding, second.implicitHandleWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, first.implicitHandleHeight + topPadding + bottomPadding, second.implicitHandleHeight + topPadding + bottomPadding)

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    first.handle: Union.StyledRectangle {
        Union.Element.type: "Handle"
        Union.Element.hints: ["first"]
        Union.Element.states {
            hovered: control.first.hovered
            activeFocus: control.first.handle.activeFocus
            enabled: control.enabled
            pressed: control.first.pressed
        }
        x: control.leftPadding + (control.horizontal ? control.first.visualPosition * (control.availableWidth - width) : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : control.first.visualPosition * (control.availableHeight - height))
    }

    second.handle: Union.StyledRectangle {
        Union.Element.type: "Handle"
        Union.Element.hints: ["second"]
        Union.Element.states {
            hovered: control.second.hovered
            activeFocus: control.second.handle.activeFocus
            enabled: control.enabled
            pressed: control.second.pressed
        }
        x: control.leftPadding + (control.horizontal ? control.second.visualPosition * (control.availableWidth - width) : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : control.second.visualPosition * (control.availableHeight - height))
    }

    background: Union.StyledRectangle {
        x: control.leftPadding + (control.horizontal ? 0 : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : 0)
        scale: control.horizontal && control.mirrored ? -1 : 1

        Union.StyledRectangle {
            Union.Element.type: "Fill"
            readonly property int smallerValue: Math.min(parent.width, parent.height)
            x: control.horizontal ? control.first.position * parent.width + (smallerValue / 2) : 0
            y: control.horizontal ? 0 : control.second.visualPosition * parent.height + (smallerValue / 2)
            width: control.horizontal ? control.second.position * parent.width - control.first.position * parent.width - smallerValue : smallerValue
            height: control.horizontal ? smallerValue : control.second.position * parent.height - control.first.position * parent.height - smallerValue
        }
    }
}
