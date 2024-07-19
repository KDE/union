// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Noah Davis <noahadvs@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import org.kde.union.impl as Union

T.ToolBar {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    Union.Element.type: "ToolBar"
    Union.Element.colorSet: Union.ColorSet.Window
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
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

    background: Union.Background {}
}
