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

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.margins.left
    rightInset: Union.Style.properties.layout.margins.right
    topInset: Union.Style.properties.layout.margins.top
    bottomInset: Union.Style.properties.layout.margins.bottom

    font: Union.Style.properties.text.font

    color: Union.Style.properties.text.color
    selectionColor: control.palette.highlight
    selectedTextColor: control.palette.highlightedText
    placeholderTextColor: control.palette.placeholderText

    horizontalAlignment: Union.Style.properties.text.alignment & Qt.AlignHorizontal_Mask
    verticalAlignment: Union.Style.properties.text.alignment & Qt.AlignHorizontal_Mask

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

    background: Union.StyledRectangle {}
}
