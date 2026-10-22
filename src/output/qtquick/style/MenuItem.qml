// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

import "private" as P

T.MenuItem {
    id: control

    Union.Element.type: "MenuItem"
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
        Union.ElementHint {
            name: "exclusive";
            when: {
                if (control.autoExclusive) {
                    return true
                } else if (control.action) {
                    let group = control.action.T.ActionGroup.group
                    return group?.exclusive ?? false
                } else {
                    let group = control.T.ButtonGroup.group
                    return group?.exclusive ?? false
                }
            }
        },
        Union.ElementHint { name: "with-submenu"; when: control.subMenu !== null }
    ]
    Union.Mnemonics.enabled: control.enabled && control.visible
    Union.Mnemonics.controlType: Union.Mnemonics.MenuItem
    Union.Mnemonics.label: control.text

    Union.Positioner.positionItems: [contentItem, indicator, arrow, shortcutText]

    hoverEnabled: Application.styleHints.useHoverEffects

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: {
        if (visible) {
            return Math.max(implicitBackgroundHeight + topInset + bottomInset,
                            implicitIndicatorHeight + topPadding + bottomPadding,
                            implicitContentHeight + topPadding + bottomPadding)
        } else {
            return 0
        }
    }

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
        color: (contentItem as P.DefaultContentItem)?.iconColor ?? "black"
        name: (contentItem as P.DefaultContentItem)?.iconName ?? ""
        source: (contentItem as P.DefaultContentItem)?.iconSource ?? Qt.url()
    }

    implicitTextPadding: (checkable ? indicator.implicitWidth : 0) + (icon.name ? icon.width : 0) + (icon.name && checkable ? control.spacing * 2 : control.spacing)

    contentItem: P.DefaultContentItem { control: control }

    indicator: P.DefaultIndicator { }

    arrow: P.DefaultIndicator { Union.Element.subElement: "Arrow" }

    Text {
        id: shortcutText
        Union.Element.subElement: "ShortcutText"

        readonly property real spacing: Union.Style.properties.layout.spacing ?? 0.0

        Shortcut {
            id: itemShortcut
            sequence: (shortcutText.visible && control.action !== null) ? control.action.shortcut : ""
        }

        text: visible ? itemShortcut.nativeText : ""
        font: Union.Style.properties.text.font.qFont
        color: Union.Style.properties.display.color ?? control.palette.text
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        visible: control.action && control.action.shortcut !== undefined && (Union.Style.properties.display.visible ?? true)
        opacity: Union.Style.properties.display.opacity ?? 1.0
    }

    background: Union.StyledRectangle { }

    // Workaround for QTBUG-142697: Text padding does not receive change
    // notifications correctly.
    Component.onCompleted: {
        Qt.callLater(control.textPaddingChanged)
    }
}
