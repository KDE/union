// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.ScrollIndicator {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    Union.Element.type: "ScrollIndicator"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.active
        enabled: control.enabled
    }
    Union.Element.attributes: {
        "orientation": control.horizontal ? "horizontal" : "vertical"
    }

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding:  Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    contentItem: Union.StyledRectangle {
        Union.Element.type: "Handle"
        Union.Element.states {
            hovered: control.hovered
            activeFocus: control.activeFocus
            visualFocus: control.visualFocus
            pressed: control.active
            enabled: control.enabled
        }

        visible: control.size < 1.0
    }

    background: Union.StyledRectangle { }
}
