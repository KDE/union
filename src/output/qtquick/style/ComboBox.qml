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
    Union.Element.hints: [
        Union.ElementHint { name: "flat"; when: control.flat },
        Union.ElementHint { name: "editable"; when: control.editable },
    ]

    Union.Positioner.positionItems: [contentItem, indicator]

    hoverEnabled: Application.styleHints.useHoverEffects
    wheelEnabled: true

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding, Union.Positioner.implicitWidth)
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

    implicitContentWidthPolicy: T.ComboBox.WidestText

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

        clip: true

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

        rightPadding: contentItem.ScrollBar.vertical.visible ? contentItem.ScrollBar.vertical.width : 0

        contentItem: ListView {
            implicitWidth: contentItem.visibleChildren.reduce((acc, item) => Math.max(acc, item.implicitWidth), 0)
            implicitHeight: contentHeight
            model: control.delegateModel
            currentIndex: control.highlightedIndex
            highlightMoveDuration: 0

            ScrollBar.vertical: ScrollBar {
                anchors.left: parent.right
            }
        }
    }
}
