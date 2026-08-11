// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.ScrollView {
    id: control

    Union.Element.type: "ScrollView"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        enabled: control.enabled
    }
    Union.Element.hints: [
        Union.ElementHint { name: "horizontal-scroll"; when: control.ScrollBar.horizontal.visible },
        Union.ElementHint { name: "vertical-scroll"; when: control.ScrollBar.vertical.visible },
        Union.ElementHint { name: "framed-background"; when: control.Union.StyleHints.showFramedBackground },
    ]

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    // Scroll bar space added here, not in the style, so scrollAreaLayout sees unpadded size.
    leftPadding: (Union.Style.properties.layout.padding.left ?? 0.0)
        + (control.mirrored ? scrollAreaLayout.reservedWidth : 0.0)
    rightPadding: (Union.Style.properties.layout.padding.right ?? 0.0)
        + (control.mirrored ? 0.0 : scrollAreaLayout.reservedWidth)
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: (Union.Style.properties.layout.padding.bottom ?? 0.0)
        + scrollAreaLayout.reservedHeight

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    background: Union.StyledRectangle { }

    ScrollBar.vertical: ScrollBar {
        parent: control
        autoVisible: false
        visible: scrollAreaLayout.verticalVisible
        x: control.mirrored
            ? control.leftPadding - scrollAreaLayout.reservedWidth
            : control.width - control.rightPadding
        y: control.topPadding
        height: control.availableHeight
        active: control.ScrollBar.horizontal.active
    }

    ScrollBar.horizontal: ScrollBar {
        parent: control
        autoVisible: false
        visible: scrollAreaLayout.horizontalVisible
        x: control.leftPadding
        y: control.height - control.bottomPadding
        width: control.availableWidth
        active: control.ScrollBar.vertical.active
    }

    data: [
        Union.WheelHandler {
            target: control.contentItem
        },
        Union.ScrollAreaLayout {
            id: scrollAreaLayout
            flickable: control.contentItem
            verticalScrollBar: control.ScrollBar.vertical
            horizontalScrollBar: control.ScrollBar.horizontal
        }
    ]
}
