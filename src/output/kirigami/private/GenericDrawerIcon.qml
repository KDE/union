/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *  SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import org.kde.union.impl as Union
import org.kde.kirigami as Kirigami

Item {
    id: control
    Union.Element.type: "GenericDrawerIcon"
    property Kirigami.OverlayDrawer drawer
    property color color: Union.Style.properties.text.color
    layer.enabled: true

    Kirigami.Icon {
        selected: drawer.handle.pressed
        opacity: 1 - drawer.position
        anchors.fill: parent
        source: drawer.handleClosedIcon.name ? drawer.handleClosedIcon.name : drawer.handleClosedIcon.source
        color: drawer.handleClosedIcon.color
    }
    Kirigami.Icon {
        selected: drawer.handle.pressed
        opacity: drawer.position
        anchors.fill: parent
        source: drawer.handleOpenIcon.name ? drawer.handleOpenIcon.name : drawer.handleOpenIcon.source
        color: drawer.handleOpenIcon.color
    }
}

