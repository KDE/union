/*
 * SPDX-FileCopyrightText: 2017 Marco Martin <notmart@gmail.com>
 * SPDX-FileCopyrightText: 2020 Nate Graham <nate@kde.org>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Templates as T
import org.kde.union.impl as Union

T.SpinBox {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             up.implicitIndicatorHeight, down.implicitIndicatorHeight)

    wheelEnabled: true
    hoverEnabled: Application.styleHints.useHoverEffects

    Union.Element.type: "SpinBox"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus || (control.contentItem.activeFocus && (
            control.contentItem.focusReason == Qt.TabFocusReason ||
            control.contentItem.focusReason == Qt.BacktabFocusReason ||
            control.contentItem.focusReason == Qt.ShortcutFocusReason
        ))
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

    spacing: Union.Style.layout.horizontalSpacing

    validator: IntValidator {
        locale: control.locale.name
        bottom: Math.min(control.from, control.to)
        top: Math.max(control.from, control.to)
    }

    contentItem: T.TextField {
        id: textField
        opacity: enabled ? 1 : 0.5
        implicitWidth: Math.ceil(contentWidth) + leftPadding + rightPadding
        implicitHeight: Math.ceil(contentHeight) + topPadding + bottomPadding
        text: control.displayText
        font: control.font
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.textColor
        selectionColor: Kirigami.Theme.highlightColor
        selectedTextColor: Kirigami.Theme.highlightedTextColor
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: control.inputMethodHints
        selectByMouse: true
        hoverEnabled: false
    }

    up.indicator: Union.Background {}

    down.indicator: Union.Background {}

    background: Union.Background {}
}
