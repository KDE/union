// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.Drawer {
    id: control

    Union.Element.type: "Drawer"
    Union.Element.hints: modal ? ["modal"] : []
    Union.Element.attributes: {
        let result = {}
        switch (edge) {
            case Qt.LeftEdge:
                result.edge = "left"
                break
            case Qt.RightEdge:
                result.edge = "right"
                break
            case Qt.TopEdge:
                result.edge = "top"
                break
            case Qt.BottomEdge:
                result.edge = "bottom"
                break
        }
        return result
    }

    parent: T.Overlay.overlay

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: SafeArea.margins.left + Union.Style.properties.layout.padding.left
    rightPadding: SafeArea.margins.right + Union.Style.properties.layout.padding.right
    topPadding: SafeArea.margins.top + Union.Style.properties.layout.padding.top
    bottomPadding: SafeArea.margins.bottom + Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    font: Union.Style.properties.text.font

    spacing: Union.Style.properties.layout.spacing

    enter: Transition { SmoothedAnimation { velocity: 5 } }
    exit: Transition { SmoothedAnimation { velocity: 5 } }

    background: Union.StyledRectangle { }

    T.Overlay.modal: Union.StyledRectangle {
        Union.Element.type: "Overlay"
        Union.Element.hints: ["drawer", "modal"]
    }

    T.Overlay.modeless: Union.StyledRectangle {
        Union.Element.type: "Overlay"
        Union.Element.hints: ["drawer", "modeless"]
    }
}
