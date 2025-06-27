// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
