// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.HorizontalHeaderView {
    id: control
    Union.Element.type: "HeaderView"
    Union.Element.states {
        activeFocus: control.activeFocus
        enabled: control.enabled
    }
    
    implicitWidth: syncView ? syncView.width : 0
    implicitHeight: Math.max(1, contentHeight)
    
    delegate: Union.StyledRectangle {
        id: delegate

        required property var model

        Union.Element.type: "HeaderViewDelegate"
        Union.Element.states {
            activeFocus: delegate.activeFocus
            enabled: delegate.enabled
        }
        Union.Element.hints: [
            Union.ElementHint { name: "sort-ascending"; when: delegate.model.hasOwnProperty("sort") && delegate.model.sort == Qt.AscendingOrder },
            Union.ElementHint { name: "sort-descending"; when: delegate.model.hasOwnProperty("sort") && delegate.model.sort != Qt.AscendingOrder },
        ]

        implicitWidth: Math.max(Union.Style.properties.layout.width, Union.Positioner.implicitWidth)
        implicitHeight: Math.max(Union.Style.properties.layout.height, Union.Positioner.implicitHeight)

        Union.Positioner.positionItems: [text, icon]

        Text {
            id: text

            Union.PositionedItem.source: Union.PositionerSource.Text

            text: delegate.model[control.textRole]
            font: Union.Style.properties.text.font ?? Application.font
            color: Union.Style.properties.text.color
        }

        Union.Icon {
            id: icon

            Union.PositionedItem.source: Union.PositionerSource.Icon

            color: Union.Style.properties.icon.color
            implicitWidth: Union.Style.properties.icon.width
            implicitHeight: Union.Style.properties.icon.height
            name: Union.Style.properties.icon.name
            visible: name
        }
    }
}
