// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>


import QtQuick
import QtQuick.Controls.impl
import QtQuick.Shapes
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.SelectionRectangle {
    id: control
    
    topLeftHandle: Handle {}
    bottomRightHandle: Handle {}
    
    component Handle : Union.StyledRectangle {
        // This is not a descendant of the main selection rectangle, so it needs it's own type
        Union.Element.type: "SelectionRectangleHandle"
        Union.Element.states {
            enabled: SelectionRectangle.control.enabled
            pressed: SelectionRectangle.dragging
        }
        visible: SelectionRectangle.control.active
    }
}
