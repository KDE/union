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

    font: Union.Style.properties.text.font ?? Application.font

    Union.Positioner.positionItems: [searchIndicator.indicator, contentItem, clearIndicator.indicator]

    delegate: ItemDelegate {
        width: ListView.view.width
        text: model[control.textRole]
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled

        required property var model
        required property int index
    }

    searchIndicator.indicator: Union.Icon {
        Union.Element.type: "SearchIndicator"
        color: Union.Style.properties.icon.color
        implicitWidth: Union.Style.properties.icon.width
        implicitHeight: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }

    clearIndicator.indicator: Union.Icon {
        Union.Element.type: "ClearIndicator"
        color: Union.Style.properties.icon.color
        implicitWidth: Union.Style.properties.icon.width
        implicitHeight: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
        visible: control.text.length > 0
    }

    contentItem: T.TextField {
        text: control.text
        placeholderText: control.placeholderText
        selectByMouse: control.selectTextByMouse
        color: control.palette.text
        selectionColor: control.palette.highlight
        selectedTextColor: control.palette.highlightedText
        horizontalAlignment: Union.Alignment.toQtHorizontal(control.Union.Style.properties.text.alignment.horizontal)
        verticalAlignment: Union.Alignment.toQtVertical(control.Union.Style.properties.text.alignment.vertical)

        Text {
            id: placeholder
            x: parent.leftPadding
            y: parent.topPadding
            width: parent.width - parent.leftPadding - parent.rightPadding
            height: parent.height - parent.topPadding - parent.bottomPadding

            text: control.placeholderText
            font: parent.font
            color: control.palette.placeholderText
            visible: !parent.length && !parent.preeditText && (!parent.activeFocus || parent.horizontalAlignment !== Qt.AlignHCenter)
            verticalAlignment: parent.verticalAlignment
            renderType: parent.renderType
            elide: parent.wrapMode == Text.NoWrap ? Text.ElideRight : Text.ElideNone
        }

        T.ContextMenu.menu: P.TextFieldContextMenu {
            target: control
        }
    }

    popup: Popup {
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
