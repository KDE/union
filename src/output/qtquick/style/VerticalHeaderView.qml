// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.VerticalHeaderView {
    id: control
    Union.Element.type: "HeaderView"
    Union.Element.states {
        activeFocus: control.activeFocus
        enabled: control.enabled
    }
    
    implicitWidth: Math.max(1, contentWidth)
    implicitHeight: syncView ? syncView.height : 0
    
    delegate: Union.StyledRectangle {
        id: delegate
        Union.Element.type: "HeaderViewDelegate"
        Union.Element.states {
            enabled: control.enabled
        }

        required property var model

        implicitWidth: Math.max(control.width, text.implicitWidth)
        implicitHeight: text.implicitHeight

        Text {
            id: text
            anchors.fill: parent
            enabled: parent.enabled
            horizontalAlignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal)
            verticalAlignment: Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)
            text: delegate.model[control.textRole]
            color: Union.Style.properties.text.color
        }
    }
}
