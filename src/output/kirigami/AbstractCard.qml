// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick

import org.kde.kirigami.templates as KT

import org.kde.union.impl as Union

KT.AbstractCard {
    id: control

    Union.Element.type: "Card"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.down
        checked: control.checked
        enabled: control.enabled
        highlighted: control.highlighted
    }

    background: Union.StyledRectangle {
        // Workaround for Card expecting background to be a QtQuick Rectangle.
        property real radius: Union.Style.properties.corners.topLeft.radius ?? 0.0
        property QtObject border: QtObject {
            property real width: Union.Style.properties.border.left.size ?? 0.0
        }
    }
}
