/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Templates as T

import org.kde.kirigami.templates as KT

import org.kde.union.impl as Union

KT.AbstractApplicationHeader {
    id: root

    Union.Element.type: "ApplicationHeader"
    Union.Element.hints: [
        Union.ElementHint {
            name: "with-separator";
            when: root.separatorVisible && (!root.page || !root.page.header || !root.page.header.visible || !(root.page.header instanceof T.ToolBar))
        }
    ]

    background: Union.StyledRectangle {
        DragHandler {
            target: null
            onActiveChanged: {
                if (active) {
                    root.Window.window.startSystemMove()
                }
            }
        }
    }
}
