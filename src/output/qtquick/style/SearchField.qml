// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2025 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

import "private" as P

T.SearchField {
    id: control

    Union.Element.type: "SearchField"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.activeFocus &&
                    (control.focusReason === Qt.TabFocusReason ||
                     control.focusReason === Qt.BacktabFocusReason ||
                     control.focusReason === Qt.ShortcutFocusReason)
        enabled: control.enabled
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, Union.Positioner.implicitHeight)

    hoverEnabled: Application.styleHints.useHoverEffects

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    spacing: Union.Style.properties.layout.spacing

    font: Union.Style.properties.text.font.qFont ?? Application.font

    Union.Positioner.positionItems: [searchIndicator.indicator, contentItem, clearIndicator.indicator]

    delegate: ItemDelegate {
        width: ListView.view.width
        text: model[control.textRole]
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled

        required property var model
        required property int index
    }

    searchIndicator.indicator: P.DefaultIndicator {
        Union.Element.subElement: "Search"
        Union.Element.states {
            hovered: control.searchIndicator.hovered
            pressed: control.searchIndicator.pressed
            enabled: control.searchIndicator.indicator.enabled
        }
    }

    clearIndicator.indicator: P.DefaultIndicator {
        Union.Element.subElement: "Clear"
        Union.Element.states {
            hovered: control.clearIndicator.hovered
            pressed: control.clearIndicator.pressed
            enabled: control.clearIndicator.indicator.enabled
        }

        visible: control.text && (Union.Style.properties.display.visible ?? true)
    }

    contentItem: T.TextField {
        Union.Element.subElement: "Text"

        text: control.text
        placeholderText: control.placeholderText
        selectByMouse: control.selectTextByMouse
        color: Union.Style.properties.display.color ?? control.palette.text
        selectionColor: control.palette.highlight
        selectedTextColor: control.palette.highlightedText
        horizontalAlignment: Union.Style.properties.text.alignment ?? Qt.AlignLeft
        // verticalAlignment: Union.Alignment.toQtVertical(control.Union.Style.properties.text.alignment.vertical)

        P.PlaceholderText {
            id: placeholder
            control: parent
        }
    }

    T.ContextMenu.menu: P.TextFieldContextMenu {
        target: control
    }

    popup: Popup {
        Union.Element.hints: Union.ElementHint { name: "SearchField" }

        y: control.height
        width: control.width
        height: Math.min(contentItem.implicitHeight, control.Window.height - control.y - control.height - control.padding)
        palette: control.palette

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.delegateModel
            currentIndex: control.highlightedIndex
            highlightMoveDuration: 0
            boundsBehavior: ListView.StopAtBounds

            T.ScrollIndicator.vertical: ScrollIndicator {}
        }
    }

    background: Union.StyledRectangle {}
}
