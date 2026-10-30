// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
// SPDX-FileCopyrightText: 2024 Noah Davis <noahadvs@gmail.com>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

import "private" as P

T.CheckDelegate {
    id: control

    property bool __alternatingColors: (TableView.view?.alternatingRows && typeof row !== "undefined" && row % 2) || (Union.StyleHints.useAlternatingColors && typeof index !== "undefined" && index % 2)

    Union.Element.type: "CheckDelegate"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.pressed
        checked: control.checked
        enabled: control.enabled
        highlighted: control.highlighted
    }
    Union.Element.hints: [
        Union.ElementHint { name: "with-icon"; when: control.icon.name || control.icon.source.toString() },
        Union.ElementHint { name: "hover-enabled"; when: control.hoverEnabled },
        Union.ElementHint { name: "inside-list"; when: (control.ListView?.view || control.parent?.ListView.view) ?? false },
        Union.ElementHint { name: "inside-table"; when: (control.TableView?.view || control.parent?.TableView.view) ?? false },
        Union.ElementHint { name: "alternating-colors"; when: control.__alternatingColors },
        Union.ElementHint { name: "changed"; when: false },
        Union.ElementHint { name: "last-in-list"; when: typeof index !== "undefined" && control.ListView.view && index == control.ListView.view.count - 1 },
    ]
    Union.Element.attributes: P.DisplayAttribute { control: control }

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

    font: Union.Style.properties.text.font.qFont

    spacing: Union.Style.properties.layout.spacing

    hoverEnabled: Application.styleHints.useHoverEffects

    icon {
        width: (contentItem as P.DefaultContentItem)?.iconWidth ?? 0.0
        height: (contentItem as P.DefaultContentItem)?.iconHeight ?? 0.0
        color: (contentItem as P.DefaultContentItem)?.iconColor ?? "transparent"
    }

    Union.Positioner.positionItems: [contentItem, indicator]

    indicator: P.DefaultIndicator { }

    contentItem: P.DefaultContentItem { control: control }

    background: Union.StyledRectangle {}
}
