// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Templates as T
import org.kde.union.impl as Union

T.Popup {
    id: control

    Union.Element.type: "Popup"
    Union.Element.states {
        activeFocus: control.activeFocus
        enabled: control.enabled
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding:  Union.Style.properties.layout.padding.bottom

    background: Union.StyledRectangle {
        // Explicitly bind to the style instance of the main Popup item.
        // This ensures that even when the popup is a Window type or the content
        // otherwise loses connection to the main Dialog item, we still use that
        // item's properties rather than an unknown instance.
        style: control.Union.Style
    }

    T.Overlay.modal: Union.StyledRectangle {
        Union.Element.type: "Overlay"
        Union.Element.hints: Union.ElementHint { name: "modal" }
    }

    T.Overlay.modeless: Union.StyledRectangle {
        Union.Element.type: "Overlay"
        Union.Element.hints: Union.ElementHint { name: "modeless" }
    }
}
