// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.BusyIndicator {
    id: control

    Union.Element.type: "BusyIndicator"

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding:  Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    contentItem: Item {
        implicitWidth: Union.Style.properties.layout.width ?? 0
        implicitHeight: Union.Style.properties.layout.height ?? 0

        Union.Icon {
            anchors.centerIn: parent
            width: Math.min(parent.width, parent.height)
            height: width
            name: Union.Style.properties.icon.name
            color: Union.Style.properties.icon.color
            opacity: control.running ? 1 : 0

            RotationAnimator on rotation {
                running: control.running
                loops: Animation.Infinite

                property int rotationStart: ((new Date().valueOf() % duration) / duration) * 360;

                from: rotationStart
                to: rotationStart + 360
                duration: 2000
            }
        }
    }
}
