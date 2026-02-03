// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
// SPDX-FileCopyrightText: 2024 Noah Davis <noahadvs@gmail.com>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

import "private" as P

T.ItemDelegate {
    id: control

    Union.Element.type: "ItemDelegate"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.down
        checked: control.checked
        enabled: control.enabled
        highlighted: control.highlighted
    }
    Union.Element.hints: [
        Union.ElementHint { name: "with-icon"; when: control.icon.name || control.icon.source.toString() },
        Union.ElementHint { name: "hoverEnabled"; when: control.hoverEnabled },
        Union.ElementHint { name: "insideList"; when: control.ListView?.view },
        Union.ElementHint { name: "insideTable"; when: control.TableView?.view },
        Union.ElementHint {
            name: "useAlternateBackgroundColor"
            when: (TableView.view?.alternatingRows && control.row % 2)
                  || (Union.OutputProperties.useAlternatingColors && control.index % 2)
        }
    ]
    Union.Element.attributes: P.DisplayAttribute { control: control }

    hoverEnabled: Application.styleHints.useHoverEffects

    implicitWidth: Math.max(implicitContentWidth + leftPadding + rightPadding, Math.max(implicitBackgroundWidth + leftInset + rightInset, Union.Positioner.implicitWidth))
    implicitHeight: Math.max(implicitContentHeight + topPadding + bottomPadding, Math.max(implicitBackgroundHeight + topInset + bottomInset, Union.Positioner.implicitHeight))

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding: Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    spacing: Union.Style.properties.layout.spacing

    font: Union.Style.properties.text.font

    icon {
        color: Union.Style.properties.icon.color
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }

    Union.Positioner.positionItems: [contentItem]

    contentItem: P.DefaultContentItem { control: control }

    background: Union.StyledRectangle {}
}
