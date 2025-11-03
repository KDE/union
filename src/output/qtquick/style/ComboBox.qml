// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.ComboBox {
    id: control

    Union.Element.type: "ComboBox"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.down
        enabled: control.enabled
        highlighted: control.highlighted
    }
    Union.Element.hints: {
        let result = [];
        if (flat) {
            result.push("flat");
        }
        if (editable) {
            result.push("editable");
        }
        return result;
    }
    Union.Positioner.positionItems: [contentItem, indicator]

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
    spacing: Union.Style.properties.layout.spacing

    delegate: ItemDelegate {
        required property var model
        required property int index

        width: ListView.view.width
        text: model[control.textRole]
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
    }

    indicator: Union.Icon {
        Union.PositionedItem.source: Union.PositionerSource.Icon
        implicitWidth: Union.Style.properties.icon.width
        implicitHeight: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        color: Union.Style.properties.icon.color
    }

    contentItem: TextInput {
        Union.PositionedItem.source: Union.PositionerSource.Text
        color: Union.Style.properties.text.color
        text: control.editable ? control.editText : control.displayText
        enabled: control.editable
        autoScroll: control.editable
        readOnly: control.down
        inputMethodHints: control.inputMethodHints
        validator: control.validator
        selectByMouse: control.selectTextByMouse
        selectionColor: control.palette.highlight
        selectedTextColor: control.palette.highlightedText
   }

    background: Union.StyledRectangle { }

    popup: Popup {
        Union.Element.hints: ["combobox"]

        y: control.height
        width: Math.max(control.width, contentItem.implicitWidth)
        height: Math.min(contentItem.implicitHeight + topPadding + bottomPadding, control.Window.height)

        contentItem: ListView {
            clip: true
            implicitWidth: {
                let max = 0
                for (let i of contentItem.visibleChildren) {
                    max = Math.max(i.implicitWidth, max)
                }
                return max
            }
            implicitHeight: contentHeight
            model: control.delegateModel
            currentIndex: control.highlightedIndex
            highlightMoveDuration: 0
            T.ScrollIndicator.vertical: ScrollIndicator {}
        }
    }
}
