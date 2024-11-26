// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
// import QtQuick.Controls.impl
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.ScrollIndicator {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    Union.Element.type: "ScrollIndicator"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.active
        enabled: control.enabled
    }
    Union.Element.attributes: {
        "orientation": control.horizontal ? "horizontal" : "vertical"
    }

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding:  Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    contentItem: Union.StyledRectangle {
        // implicitWidth: 2
        // implicitHeight: 2
        //
        // color: control.palette.mid
        // visible: control.size < 1.0
        // opacity: 0.0
        //
        // states: State {
        //     name: "active"
        //     when: control.active
        //     PropertyChanges { control.contentItem.opacity: 0.75 }
        // }
        //
        // transitions: [
        //     Transition {
        //         from: "active"
        //         SequentialAnimation {
        //             PauseAnimation { duration: 450 }
        //             NumberAnimation { target: control.contentItem; duration: 200; property: "opacity"; to: 0.0 }
        //         }
        //     }
        // ]
    }

    background: Union.StyledRectangle { }
}
