// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

/*
 * A standardised form of ElementAttribute to be used to expose the "display"
 * property of AbstractButton.
 */
Union.ElementAttribute {
    property T.AbstractButton control

    name: "display"
    value: switch (control.display) {
        case T.AbstractButton.IconOnly: return "icon-only"
        case T.AbstractButton.TextOnly: return "text-only"
        case T.AbstractButton.TextBesideIcon: return "text-beside-icon"
        case T.AbstractButton.TextUnderIcon: return "text-under-icon"
    }
}
