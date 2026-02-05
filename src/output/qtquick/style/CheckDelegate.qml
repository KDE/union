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

    property bool __alternatingColors: (TableView.view?.alternatingRows && row % 2) || (Union.OutputProperties.useAlternatingColors && index % 2)

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
        Union.ElementHint { name: "inside-list"; when: control.ListView?.view },
        Union.ElementHint { name: "inside-table"; when: control.TableView?.view },
        Union.ElementHint { name: "alternating-colors"; when: control.__alternatingColors },
    ]
    Union.Element.attributes: P.DisplayAttribute { control: control }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding: Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    font: Union.Style.properties.text.font

    spacing: Union.Style.properties.layout.spacing

    hoverEnabled: Application.styleHints.useHoverEffects

    icon {
        color: palette.text
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }

    Union.Positioner.positionItems: [contentItem, indicator]

    indicator: Union.StyledRectangle {
        Union.Element.type: "Indicator"
    }

    contentItem: P.DefaultContentItem { control: control }

    background: Union.StyledRectangle {}
}
