// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import org.kde.union.impl as Union
import org.kde.kirigami as Kirigami

Union.Icon {
    id: control
    Union.Element.type: "MenuIcon"
    property Kirigami.OverlayDrawer drawer
    Union.Element.hints: drawer.opened ? ["opened"] : []
    width: Union.Style.properties.icon.width
    height: Union.Style.properties.icon.height
    name: Union.Style.properties.icon.name
}
