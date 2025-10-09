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
    
    Union.Element.type: "SelectionRectangle"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        enabled: control.enabled
    }
    
    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding: Union.Positioner.padding.bottom
    
    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom
    
    topLeftHandle: Handle {}
    bottomRightHandle: Handle {}
    
    component Handle : Union.StyledRectangle {
        // This is not a descendant of the main selection rectangle, so it needs it's own type
        Union.Element.type: "SelectionRectangleHandle"
        Union.Element.states {
            enabled: SelectionRectangle.control.enabled
            visualFocus: SelectionRectangle.dragging
        }
        visible: SelectionRectangle.control.active
    }
    
}