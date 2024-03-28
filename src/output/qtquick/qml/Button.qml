/*
    SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
    SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>

    SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Templates as T

// import org.kde.kirigami 2.12 as Kirigami

import org.kde.union as Union

T.Button {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    hoverEnabled: true

    leftPadding: elementInfo.leftPadding
    rightPadding: elementInfo.rightPadding
    topPadding: elementInfo.topPadding
    bottomPadding: elementInfo.bottomPadding

    font: elementInfo.font

    contentItem: Text {
        text: control.text
        font: control.font
        color: elementInfo.color

        verticalAlignment: Qt.AlignVCenter
    }

    background: Union.Background {
        element: elementInfo

    }

    Union.Element {
        id: elementInfo
        name: "button"
        hovered: control.hovered
        focus: control.focus
        activeFocus: control.activeFocus
        pressed: control.pressed
    }
}

