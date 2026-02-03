// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T

import org.kde.union.impl as Union

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
        Union.ElementHint { name: "constrained"; when: priv.constrained },
    ]

    hoverEnabled: Application.styleHints.useHoverEffects

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, Union.Positioner.implicitHeight)

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
        id: upIndicator

        Union.Element.type: "Indicator"
        Union.Element.states {
            hovered: control.up.hovered
            pressed: control.up.pressed
            enabled: upIndicator.enabled
        }
        Union.Element.hints: Union.ElementHint { name: "increase" }

        Union.Icon {
            anchors.centerIn: parent
            width: Union.Style.properties.icon.width
            height: Union.Style.properties.icon.height
            name: Union.Style.properties.icon.name
            color: Union.Style.properties.icon.color
        }
    }

    down.indicator: Union.StyledRectangle {
        id: downIndicator

        Union.Element.type: "Indicator"
        Union.Element.states {
            hovered: control.down.hovered
            pressed: control.down.pressed
            enabled: downIndicator.enabled
        }
        Union.Element.hints: Union.ElementHint { name: "decrease" }

        Union.Icon {
            anchors.centerIn: parent
            width: Union.Style.properties.icon.width
            height: Union.Style.properties.icon.height
            name: Union.Style.properties.icon.name
            color: Union.Style.properties.icon.color
        }
    }

    background: Union.StyledRectangle { }

    QtObject {
        id: priv

        // Measure what the maximum length of text is that needs to fit.
        property TextMetrics metrics: TextMetrics {
            font: control.font
            text: control.textFromValue(control.to, Qt.locale())
        }

        // Helper to retrieve the size of indicators when not constrained.
        // Since the indicator size may changed based on the constrained hint,
        // we need a baseline to use in comparisons.
        property Item unconstrained: Item {
            Union.Element.type: "SpinBox"

            Item {
                id: unconstrainedIndicator
                Union.Element.type: "Indicator"
                width: Union.Style.properties.layout.width
            }
        }

        // Determine if we are "space constrained", that is, the text that gets
        // displayed wouldn't fit if both indicators are displayed next to each
        // other. This can be used by styles to switch to a more compact layout
        // in that case.
        property bool constrained: {
            let constrained = Infinity

            // Use preferredWidth if set, or maximumWidth if preferred is not set.
            if (control.Layout.preferredWidth > 0) {
                constrained = control.Layout.preferredWidth
            } else if (control.Layout.maximumWidth < Infinity) {
                constrained = control.Layout.maximumWidth
            }

            return (metrics.width + unconstrainedIndicator.width * 2 + control.spacing * 2) > constrained
        }
    }
}
