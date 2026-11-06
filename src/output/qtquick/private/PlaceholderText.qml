// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

Text {
    required property var control

    Union.Element.hints: Union.ElementHint { name: "placeholder" }

    x: control.leftPadding
    y: control.topPadding
    width: control.width - control.leftPadding - control.rightPadding
    height: control.height - control.topPadding - control.bottomPadding

    text: control.placeholderText
    horizontalAlignment: Union.Style.properties.text.horizontalAlignment ?? control.horizontalAlignment
    verticalAlignment: Union.Style.properties.text.verticalAlignment ?? control.verticalAlignment

    font: Union.Style.properties.text.font.qFont ?? control.font
    color: Union.Style.properties.display.color ?? control.palette.placeholderText
    elide: wrapMode == Text.NoWrap ? (Union.Style.properties.text.elide ?? Text.ElideRight) : Text.ElideNone
    wrapMode: Union.Style.properties.text.wrapMode ?? Text.NoWrap

    visible: !control.length && !control.preeditText && (Union.Style.properties.display.visible ?? true)
    opacity: Union.Style.properties.display.opacity ?? 1.0
}
