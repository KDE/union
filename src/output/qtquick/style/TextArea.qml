// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Noah Davis <noahadvs@gmail.com>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

import "private" as P

T.TextArea {
    id: control

    Union.Element.type: "TextArea"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.activeFocus
                     && (control.focusReason === Qt.TabFocusReason
                         || control.focusReason === Qt.BacktabFocusReason
                         || control.focusReason === Qt.ShortcutFocusReason)
        enabled: control.enabled
    }

    hoverEnabled: Application.styleHints.useHoverEffects

    implicitWidth: Math.max(contentWidth + leftPadding + rightPadding,
                            implicitBackgroundWidth + leftInset + rightInset,
                            placeholder.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(contentHeight + topPadding + bottomPadding,
                             implicitBackgroundHeight + topInset + bottomInset,
                             placeholder.implicitHeight + topPadding + bottomPadding)

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    font: Union.Style.properties.text.font ?? Application.font
    color: Union.Style.properties.text.color ?? palette.text

    selectionColor: control.palette.highlight
    selectedTextColor: control.palette.highlightedText

    horizontalAlignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal)
    verticalAlignment: Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)

    Text {
        id: placeholder

        Union.Element.hints: Union.ElementHint { name: "placeholder" }

        x: control.leftPadding
        y: control.topPadding
        width: control.width - (control.leftPadding + control.rightPadding)
        height: control.height - (control.topPadding + control.bottomPadding)

        text: control.placeholderText
        font: Union.Style.properties.text.font ?? Application.font
        color: Union.Style.properties.text.color ?? control.palette.placeholderText
        horizontalAlignment: control.horizontalAlignment
        verticalAlignment: control.verticalAlignment
        visible: !control.length && !control.preeditText
        elide: Text.ElideRight
        renderType: control.renderType
    }

    background: Union.StyledRectangle {}

    T.ContextMenu.menu: P.TextFieldContextMenu {
        target: control
    }
}
