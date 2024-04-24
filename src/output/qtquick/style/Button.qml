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

    leftPadding: Union.Element.padding.left
    rightPadding: Union.Element.padding.right
    topPadding: Union.Element.padding.top
    bottomPadding: Union.Element.padding.bottom

    // font: elementInfo.font

    contentItem: Text {

        text: control.text
    }

    background: Union.Background {
        implicitWidth: control.Union.Element.implicitWidth
        implicitHeight: control.Union.Element.implicitHeight
        element: control.Union.Element
    }

    Union.Element.type: "Button"
    Union.Element.states {
        hovered: control.hovered
        focus: control.focus
        activeFocus: control.activeFocus
        pressed: control.pressed
        enabled: control.enabled
    }
}
