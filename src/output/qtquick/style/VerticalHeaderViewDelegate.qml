// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2025 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

import "private" as P

T.HeaderViewDelegate {
    id: control

    required property int column
    required property int row
    required model

    Union.Element.type: "HeaderViewDelegate"

    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.pressed
        enabled: control.enabled
        highlighted: control.highlighted
    }
    Union.Element.hints: [
        Union.ElementHint { name: "vertical" },
        Union.ElementHint { name: "sort-ascending"; when: control.model.sort !== undefined && control.model.sort === Qt.AscendingOrder },
        Union.ElementHint { name: "sort-descending"; when: control.model.sort !== undefined && control.model.sort !== Qt.AscendingOrder },
        Union.ElementHint { name: "last"; when: control.row >= control.headerView.rows - 1 }
    ]
    Union.Element.attributes: P.DisplayAttribute { control: control }

    Union.Positioner.positionItems: [contentItem, indicator]

    hoverEnabled: Application.styleHints.useHoverEffects

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    spacing: Union.Style.properties.layout.spacing

    font: Union.Style.properties.text.font.qFont

    highlighted: selected

    text: control.model[control.headerView.textRole]

    icon {
        width: (contentItem as P.DefaultContentItem)?.iconWidth ?? 0.0
        height: (contentItem as P.DefaultContentItem)?.iconHeight ?? 0.0
        color: (contentItem as P.DefaultContentItem)?.iconColor ?? "transparent"
    }

    contentItem: P.DefaultContentItem { control: control }

    indicator: P.DefaultIndicator { }

    background: Union.StyledRectangle { }
}
