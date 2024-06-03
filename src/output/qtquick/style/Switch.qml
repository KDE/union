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

    //TODO Union.Style.spacing?
    spacing: Union.Style.padding.left

    //TODO should icon sizes be defined also there?
   // icon.width: Kirigami.Units.iconSizes.sizeForLabels
   // icon.height: Kirigami.Units.iconSizes.sizeForLabels

    indicator: Union.Background {
        x: (control.text || control.icon.name || control.icon.source)
            ? (control.mirrored ? control.width - width - control.rightPadding : control.leftPadding)
            : control.leftPadding + Math.round((control.availableWidth - width) / 2)
        y: control.topPadding + Math.round((control.availableHeight - height) / 2)

        Union.Element.type: "SwitchIndicator"
        Union.Element.colorSet: Union.ColorSet.Button
        Union.Element.states {
            hovered: control.hovered
            focus: control.focus
            activeFocus: control.activeFocus
            visualFocus: control.visualFocus
            pressed: control.pressed
            enabled: control.enabled
            highlighted: control.checked
        }
        Union.Background {
            // FIXME: binding on x sometimes gets randomly broken
            x: Math.max(0, Math.min(parent.width - width, control.visualPosition * parent.width - (width / 2)))
            anchors.verticalCenter: parent.verticalCenter
            Union.Element.type: "Handle"
            Union.Element.colorSet: Union.ColorSet.Button
            Union.Element.states {
                hovered: control.hovered
                focus: control.focus
                activeFocus: control.activeFocus
                visualFocus: control.visualFocus
                pressed: control.pressed
                enabled: control.enabled
                highlighted: control.checked
            }
        }
        //TODO: We need a way to size background elements as the size of a third hint element called hint-bar-size
        width: 30
    }

    contentItem: T.Label {
        readonly property int effectiveIndicatorWidth: control.indicator && control.indicator.visible && control.indicator.width > 0
            ? control.indicator.width + control.spacing : 0

        leftPadding: !control.mirrored ? effectiveIndicatorWidth : 0
        rightPadding: control.mirrored ? effectiveIndicatorWidth : 0

        text: control.text
        font: control.font
        horizontalAlignment: Union.Style.text.horizontalAlignment
        verticalAlignment: Union.Style.text.verticalAlignment
    }
}

