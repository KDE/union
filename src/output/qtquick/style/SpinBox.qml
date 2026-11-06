// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T

import org.kde.union.impl as Union
import org.kde.union.private as P

T.SpinBox {
    id: control

    Union.Element.type: "SpinBox"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        enabled: control.enabled
    }
    Union.Element.hints: [
        Union.ElementHint { name: "editable"; when: control.editable },
        Union.ElementHint { name: "constrained"; when: helper.constrained },
        Union.ElementHint { name: "changed"; when: false },
        Union.ElementHint { name: "positive"; when: false },
        Union.ElementHint { name: "negative"; when: false },
    ]

    hoverEnabled: Application.styleHints.useHoverEffects

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, Union.Positioner.implicitHeight)

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    font: Union.Style.properties.text.font.qFont

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
        implicitWidth: Math.ceil(helper.maximumSize.width)
        implicitHeight: helper.maximumSize.height
        clip: true
        text: control.displayText
        font: Union.Style.properties.text.font.qFont ?? control.font
        color: Union.Style.properties.display.color ?? control.palette.text

        horizontalAlignment: Union.Style.properties.text.horizontalAlignment ?? Qt.AlignLeft
        verticalAlignment: Unoin.Style.properties.text.verticalAlignment ?? Qt.AlignVCenter

        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: control.inputMethodHints
        selectByMouse: true
        hoverEnabled: false
    }

    up.indicator: P.DefaultIndicator {
        id: upIndicator

        Union.Element.states {
            hovered: control.up.hovered
            pressed: control.up.pressed
            enabled: upIndicator.enabled
        }
        Union.Element.hints: Union.ElementHint { name: "increase" }
    }

    down.indicator: P.DefaultIndicator {
        id: downIndicator

        Union.Element.states {
            hovered: control.down.hovered
            pressed: control.down.pressed
            enabled: downIndicator.enabled
        }
        Union.Element.hints: Union.ElementHint { name: "decrease" }
    }

    background: Union.StyledRectangle { }

    P.SpinBoxHelper {
        id: helper
        control: control
    }
}
