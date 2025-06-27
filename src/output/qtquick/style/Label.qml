// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.Label {
    id: control

    Union.Element.type: "Label"

    font: Union.Style.properties.text.font
    palette: Union.Style.properties.palette.quickPalette

    color: control.palette.windowText
    linkColor: control.palette.link
}
