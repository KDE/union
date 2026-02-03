// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

import "private" as P

T.Button {
    id: control

    Union.Element.type: "Button"
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
        Union.ElementHint { name: "flat"; when: control.flat },
        // Match qqc2-desktop-style's logic.
        // TODO: Figure out how much sense this actually makes, I don't like the text condition here.
        Union.ElementHint { name: "with-menu"; when: control.Accessible.role === Accessible.ButtonMenu && control.text },
    ]
    Union.Element.attributes: P.DisplayAttribute { control: control }

    // hoverEnabled is an inherited property if it is not explicitly set.
    // hoverEnabled is used to disable hover effects of delegates when they're not interactive.
    // This also means any interactive children of those delegates will inherit hoverEnabled,
    // and their effects are disabled. For this reason we need to explicitly set it in some delegates.
    hoverEnabled: Application.styleHints.useHoverEffects

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding:  Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    font: Union.Style.properties.text.font

    spacing: Union.Style.properties.layout.spacing

    icon {
        color: Union.Style.properties.icon.color
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }

    Union.Positioner.positionItems: [contentItem, indicator]

    contentItem: P.DefaultContentItem { control: control }

    indicator: Union.Icon {
        Union.Element.type: "Indicator"
        implicitWidth: Union.Style.properties.layout.width ?? 0
        implicitHeight: Union.Style.properties.layout.height ?? 0
        name: Union.Style.properties.icon.name
        color: Union.Style.properties.icon.color
        visible: name !== ""
    }

    background: Union.StyledRectangle { }
}
