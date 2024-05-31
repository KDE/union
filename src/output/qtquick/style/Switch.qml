/*
    SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2022 ivan (@ratijas) tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.Switch {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding,
                            implicitIndicatorWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    baselineOffset: contentItem.y + contentItem.baselineOffset
    hoverEnabled: true

   // spacing: Kirigami.Units.smallSpacing

   // icon.width: Kirigami.Units.iconSizes.sizeForLabels
   // icon.height: Kirigami.Units.iconSizes.sizeForLabels

    indicator: Union.Background {
        x: (control.text || control.icon.name || control.icon.source)
            ? (control.mirrored ? control.width - width - control.rightPadding : control.leftPadding)
            : control.leftPadding + Math.round((control.availableWidth - width) / 2)
        y: control.topPadding + Math.round((control.availableHeight - height) / 2)
width: 50
height: 50
        Union.Element.type: "SwitchIndicator"
        Union.Element.colorSet: Union.ColorSet.Button
        Union.Element.states {
            hovered: control.hovered
            focus: control.focus
            activeFocus: control.activeFocus
            visualFocus: control.visualFocus
            pressed: control.pressed
            enabled: control.enabled
        }
    }

    contentItem: T.Label {
        text: control.text
        font: control.font
        horizontalAlignment: Union.Style.text.horizontalAlignment
        verticalAlignment: Union.Style.text.verticalAlignment
    }
}

