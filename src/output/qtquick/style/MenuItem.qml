// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

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
    Union.Element.hints: Union.ElementHint {
        name: "exclusive";
        when: {
            if (control.autoExclusive) {
                return true
            } else if (control.action) {
                let group = control.action.T.ActionGroup.group
                return group.exclusive
            } else {
                let group = control.T.ButtonGroup.group
                return group.exclusive
            }
        }
    }

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

    font: Union.Style.properties.text.font

    icon {
        color: Union.Style.properties.icon.color
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }

    implicitTextPadding: (checkable ? indicator.implicitWidth : 0) + (icon.name ? icon.width : 0) + (icon.name && checkable ? control.spacing * 2 : control.spacing)

    contentItem: Item {
        implicitWidth: Math.ceil(text.implicitWidth) + control.textPadding + (control.menu?.contentItem?.hasAnySubmenu ? control.spacing + control.arrow.width : 0)
        implicitHeight: Math.max(icon.implicitHeight, text.implicitHeight)

        Union.Icon {
            id: icon
            anchors {
                right: text.left
                rightMargin: control.spacing
                verticalCenter: parent.verticalCenter
            }
            control: control
            visible: name
        }

        Text {
            id: text

            anchors {
                fill: parent
                leftMargin: control.textPadding
                rightMargin: control.menu?.contentItem?.hasAnySubmenu ? control.arrow.implicitWidth + control.spacing : 0
            }

            text: control.text
            font: control.font
            color: control.palette.buttonText
            elide: Text.ElideRight
        }
    }

    indicator: Union.StyledRectangle {
        Union.Element.type: "Indicator"

        anchors {
            left: parent.left
            leftMargin: control.leftPadding
            verticalCenter: parent.verticalCenter
        }

        implicitWidth: Union.Style.properties.layout.width ?? 0
        implicitHeight: Union.Style.properties.layout.height ?? 0

        visible: control.checkable
    }

    arrow: Union.Icon {
        Union.Element.type: "Arrow"

        anchors {
            right: parent.right
            rightMargin: control.rightPadding
            verticalCenter: parent.verticalCenter
        }

        implicitWidth: Union.Style.properties.layout.width
        implicitHeight: Union.Style.properties.layout.height

        name: Union.Style.properties.icon.name
        color: Union.Style.properties.icon.color
        visible: control.subMenu
    }

    background: Union.StyledRectangle { }

    // Workaround for QTBUG-142697: Text padding does not receive change
    // notifications correctly.
    Component.onCompleted: {
        Qt.callLater(control.textPaddingChanged)
    }
}
