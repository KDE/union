// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Noah Davis <noahadvs@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import org.kde.union.impl as Union

T.TextField {
    id: control

    implicitWidth: implicitBackgroundWidth + leftInset + rightInset
                   || Math.max(contentWidth, placeholder.implicitWidth) + leftPadding + rightPadding
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding,
                             placeholder.implicitHeight + topPadding + bottomPadding)

    hoverEnabled: Application.styleHints.useHoverEffects

    Union.Element.type: "TextField"
    Union.Element.colorSet: Union.ColorSet.View
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: activeFocus
            && (control.focusReason === Qt.TabFocusReason
            || control.focusReason === Qt.BacktabFocusReason
            || control.focusReason === Qt.ShortcutFocusReason)
        enabled: control.enabled
    }

    leftPadding: Union.Style.padding.left
    rightPadding: Union.Style.padding.right
    topPadding: Union.Style.padding.top
    bottomPadding: Union.Style.padding.bottom

    leftInset: Union.Style.margins.left
    rightInset: Union.Style.margins.right
    topInset: Union.Style.margins.top
    bottomInset: Union.Style.margins.bottom

    font: Union.Style.text.font

    color: control.palette.text
    selectionColor: control.palette.highlight
    selectedTextColor: control.palette.highlightedText
    placeholderTextColor: control.palette.placeholderText
    horizontalAlignment: Union.Style.text.horizontalAlignment
    verticalAlignment: Union.Style.text.verticalAlignment

    PlaceholderText {
        id: placeholder
        x: control.leftPadding
        y: control.topPadding
        width: control.width - (control.leftPadding + control.rightPadding)
        height: control.height - (control.topPadding + control.bottomPadding)

        text: control.placeholderText
        font: control.font
        color: control.placeholderTextColor
        horizontalAlignment: control.horizontalAlignment
        verticalAlignment: control.verticalAlignment
        visible: !control.length && !control.preeditText && !control.activeFocus
        elide: Text.ElideRight
        renderType: control.renderType
    }

    background: Union.Background {}
}
