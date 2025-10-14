// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.StackView {
    id: control

    Union.Element.type: "StackView"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        enabled: control.enabled
    }

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

    spacing: Union.Style.properties.layout.spacing

    background: Union.StyledRectangle { }

    // TODO: Find some way of getting this value from input, modified by
    // animation speed multiplier.
    property int __longDuration: 400

    pushEnter: Transition {
        NumberAnimation {
            property: "x"
            from: (control.mirrored ? -0.5 : 0.5) * control.width
            to: 0
            duration: control.__longDuration
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            property: "opacity"
            from: 0.0; to: 1.0
            duration: control.__longDuration
            easing.type: Easing.OutCubic
        }
    }
    pushExit: Transition {
        NumberAnimation {
            property: "x"
            from: 0
            to: (control.mirrored ? -0.5 : 0.5) * -control.width
            duration: control.__longDuration
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            property: "opacity"
            from: 1.0; to: 0.0
            duration: control.__longDuration
            easing.type: Easing.OutCubic
        }
    }
    popEnter: Transition {
        NumberAnimation {
            property: "x"
            from: (control.mirrored ? -0.5 : 0.5) * -control.width
            to: 0
            duration: control.__longDuration
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            property: "opacity"
            from: 0.0; to: 1.0
            duration: control.__longDuration
            easing.type: Easing.OutCubic
        }
    }
    popExit: Transition {
        NumberAnimation {
            property: "x"
            from: 0
            to: (control.mirrored ? -0.5 : 0.5) * control.width
            duration: control.__longDuration
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            property: "opacity"
            from: 1.0; to: 0.0
            duration: control.__longDuration
            easing.type: Easing.OutCubic
        }
    }
    replaceEnter: Transition {
        NumberAnimation {
            property: "x"
            from: (control.mirrored ? -0.5 : 0.5) * control.width
            to: 0
            duration: control.__longDuration
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            property: "opacity"
            from: 0.0; to: 1.0
            duration: control.__longDuration
            easing.type: Easing.OutCubic
        }
    }
    replaceExit: Transition {
        NumberAnimation {
            property: "x"
            from: 0
            to: (control.mirrored ? -0.5 : 0.5) * -control.width
            duration: control.__longDuration
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            property: "opacity"
            from: 1.0; to: 0.0
            duration: control.__longDuration
            easing.type: Easing.OutCubic
        }
    }
}
