/*
    SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
    SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>

    SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Templates as T

// import org.kde.kirigami 2.12 as Kirigami

import org.kde.union.impl as Union

T.Button {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    hoverEnabled: true

    Union.Element.type: "Button"
    Union.Element.colorSet: Union.ColorSet.Button
    Union.Element.states {
        hovered: control.hovered
        focus: control.focus
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.pressed
        enabled: control.enabled
        highlighted: control.highlighted
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

    contentItem: Text {
        text: control.text
        font: control.font
        horizontalAlignment: Union.Style.text.horizontalAlignment
        verticalAlignment: Union.Style.text.verticalAlignment
    }

    background: Union.Background {
    }
}
