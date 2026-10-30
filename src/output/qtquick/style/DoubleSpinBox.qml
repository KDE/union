// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
// SPDX-FileCopyrightText: 2025 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

import "private" as P

T.DoubleSpinBox {
    id: control

    Union.Element.type: "SpinBox"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        enabled: control.enabled
    }
    Union.Element.hints: [
        Union.ElementHint { name: "doublespinbox" },
        Union.ElementHint { name: "editable"; when: control.editable },
        Union.ElementHint { name: "constrained"; when: helper.constrained },
        Union.ElementHint { name: "changed"; when: false },
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

    validator: DoubleValidator {
        locale: control.locale.name
        bottom: Math.min(control.from, control.to)
        top: Math.max(control.from, control.to)
        decimals: control.decimals
    }

    contentItem: T.TextField {
        Union.Element.subElement: "Text"

        implicitWidth: Math.ceil(Math.max(helper.fromRect.width, helper.toRect.width))
        implicitHeight: Math.ceil(Math.max(helper.fromRect.height, helper.toRect.height))
        clip: true
        text: control.displayText
        font: Union.Style.properties.text.font.qFont
        color: Union.Style.properties.display.color ?? control.palette.text

        horizontalAlignment: Union.Style.properties.text.alignment ?? Qt.AlignLeft
        verticalAlignment: Qt.AlignVCenter

        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: control.inputMethodHints
        selectByMouse: true
        hoverEnabled: false
    }

    up.indicator: P.DefaultIndicator {
        Union.Element.subElement: "Increase"
        Union.Element.states {
            hovered: control.up.hovered
            pressed: control.up.pressed
            enabled: enabled
        }
    }

    down.indicator: P.DefaultIndicator {
        Union.Element.subElement: "Decrease"
        Union.Element.states {
            hovered: control.down.hovered
            pressed: control.down.pressed
            enabled: enabled
        }
    }

    background: Union.StyledRectangle { }

    P.SpinBoxHelper {
        id: helper
        control: control
    }
}
