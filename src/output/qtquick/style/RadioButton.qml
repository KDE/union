// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

import "private" as P

T.RadioButton {
    id: control

    Union.Element.type: "RadioButton"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.pressed
        checked: control.checked
        enabled: control.enabled
    }
    Union.Element.hints: [
        Union.ElementHint { name: "with-icon"; when: control.icon.name || control.icon.source.toString() },
        Union.ElementHint { name: "changed"; when: false },
    ]
    Union.Element.attributes: P.DisplayAttribute { control: control }

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

    icon {
        width: (contentItem as P.DefaultContentItem)?.iconWidth ?? 0.0
        height: (contentItem as P.DefaultContentItem)?.iconHeight ?? 0.0
        color: (contentItem as P.DefaultContentItem)?.iconColor ?? "transparent"
    }

    Union.Positioner.positionItems: [contentItem, indicator]

    indicator: P.DefaultIndicator { }

    contentItem: P.DefaultContentItem {
        control: control
    }

    background: Union.StyledRectangle { }
}
