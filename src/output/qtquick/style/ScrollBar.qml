// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
// import QtQuick.Controls.impl
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.ScrollBar {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    Union.Element.type: "ScrollBar"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.pressed
        enabled: control.enabled
    }
    Union.Element.attributes: {
        "orientation": control.horizontal ? "horizontal" : "vertical"
    }

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding: Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    visible: (size > 0 && size < 1 && policy === T.ScrollBar.AsNeeded) || policy === T.ScrollBar.AlwaysOn
    minimumSize: orientation === Qt.Horizontal ? height / width : width / height

    contentItem: Union.StyledRectangle {
        Union.Element.type: "Handle"
        Union.Element.states {
            hovered: control.hovered
            activeFocus: control.activeFocus
            visualFocus: control.visualFocus
            pressed: control.pressed
            enabled: control.enabled
        }
    }

    background: Union.StyledRectangle { }
}
