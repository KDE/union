// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.SplitView {
    id: control

    Union.Element.type: "SplitView"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.resizing
        enabled: control.enabled
    }
    Union.Element.hints: [
        Union.ElementHint { name: "horizontal"; when: control.orientation === Qt.Horizontal },
        Union.ElementHint { name: "vertical"; when: control.orientation === Qt.Vertical },
    ]

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

    handle: Union.StyledRectangle {
        id: handle

        Union.Element.type: "Handle"

        // Extend the interactive area of the handle with the handle's margins,
        // to ensure we can set a useful interaction area even if the handle
        // itself is only something like a 1px line.
        containmentMask: Item {
            x: -handle.Union.Style.properties.layout.margins.left
            y: -handle.Union.Style.properties.layout.margins.top
            width: (handle.Union.Style.properties.layout.margins.left ?? 0) + handle.width + (handle.Union.Style.properties.layout.margins.right ?? 0)
            height: (handle.Union.Style.properties.layout.margins.top ?? 0) + handle.height + (handle.Union.Style.properties.layout.margins.bottom ?? 0)
        }

        // Expand the area where the cursor changes to include some of the area
        // of the containment mask declared above. The handle itself doesn't
        // deal with this properly. Unfortunately we only have a single "margin"
        // property, so use the lesser of the two relevant values so we at least
        // don't change the cursor in a place where you can't interact.
        HoverHandler {
            cursorShape: Qt.SplitHCursor
            margin: {
                if (control.orientation == Qt.Horizontal) {
                    return Math.min(Union.Style.properties.layout.margins.left, Union.Style.properties.layout.margins.right)
                } else {
                    return Math.min(Union.Style.properties.layout.margins.top, Union.Style.properties.layout.margins.bottom)
                }
            }
        }
    }

    // Note: Don't override `background`, it leads to broken behaviour.
}
