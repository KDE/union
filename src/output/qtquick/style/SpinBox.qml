// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Controls.impl as QQCImpl
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.SpinBox {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, Union.Positioner.implicitHeight)

    hoverEnabled: true

    Union.Element.type: "SpinBox"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        enabled: control.enabled
    }
    Union.Element.hints: control.editable ? ["editable"] : []

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding: Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    font: Union.Style.properties.text.font

    editable: true
    wheelEnabled: true

    Union.Positioner.positionItems: [
        contentItem,
        up.indicator,
        down.indicator
    ]

    validator: IntValidator {
        locale: control.locale.name
        bottom: Math.min(control.from, control.to)
        top: Math.max(control.from, control.to)
    }

    contentItem: T.TextField {
        implicitWidth: contentWidth
        implicitHeight: contentHeight

        text: control.displayText
        font: control.font
        color: control.palette.text

        horizontalAlignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal)
        verticalAlignment: Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)

        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: control.inputMethodHints
        selectByMouse: true
        hoverEnabled: false
    }

    up.indicator: Union.StyledRectangle {
        Union.Element.type: "Indicator"
        Union.Element.hints: ["increase"]

        Union.Element.states {
            hovered: control.up.hovered
            pressed: control.up.pressed
            enabled: control.up?.enabled ?? true
        }

        QQCImpl.IconImage {
            anchors.centerIn: parent
            width: Union.Style.properties.icon.width
            height: Union.Style.properties.icon.height
            name: Union.Style.properties.icon.name ?? ""
            color: Union.Style.properties.text.color
        }
    }

    down.indicator: Union.StyledRectangle {
        Union.Element.type: "Indicator"
        Union.Element.hints: ["decrease"]

        Union.Element.states {
            hovered: control.down.hovered
            pressed: control.down.pressed
            enabled: control.down?.enabled ?? true
        }

        QQCImpl.IconImage {
            anchors.centerIn: parent
            width: Union.Style.properties.icon.width
            height: Union.Style.properties.icon.height
            name: Union.Style.properties.icon.name ?? ""
            color: Union.Style.properties.text.color
        }
    }

    background: Union.StyledRectangle { }
}
