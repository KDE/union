// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.ScrollBar {
    id: control

    Union.Element.type: "ScrollBar"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.pressed
        enabled: control.enabled
    }
    Union.Element.hints: [
        Union.ElementHint { name: "horizontal"; when: control.horizontal },
        Union.ElementHint { name: "vertical"; when: !control.horizontal },
        Union.ElementHint { name: "noninteractive"; when: !control.interactive },
    ]

    hoverEnabled: Application.styleHints.useHoverEffects

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    minimumSize: horizontal ? height / width : width / height

    Binding on visible {
        delayed: true
        restoreMode: Binding.RestoreBindingOrValue
        value: control.size < 1.0 && control.size > 0 && control.policy !== T.ScrollBar.AlwaysOff && control.parent !== null
    }

    contentItem: Union.StyledRectangle {
        Union.Element.type: "Handle"
        Union.Element.states {
            hovered: control.hovered
            activeFocus: control.activeFocus
            visualFocus: control.visualFocus
            pressed: control.pressed
            enabled: control.enabled
        }
    }

    background: Union.StyledRectangle { }

    // Ported from qqc2-desktop-style
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.MiddleButton
        onPressed: event => {
            if (event.buttons & Qt.MiddleButton) {
                control.position = positionFromMouse(event);
                event.accepted = true;
            } else {
                // Let the QQuickScrollBar handle it
                mouse.accepted = false;
            }
        }

        onPositionChanged: event => {
            if (event.buttons & Qt.MiddleButton) {
                control.position = positionFromMouse(event);
                event.accepted = true;
            } else {
                // Let the QQuickScrollBar handle it
                event.accepted = false;
            }
        }

        onReleased: event => event.accepted = false;

        function positionFromMouse(mouse/*: MouseEvent*/): real {
            return Math.min(1 - control.size, Math.max(0,
                (control.horizontal
                    ? mouse.x / width
                    : mouse.y / height
                ) - control.size / 2
            ));
        }
    }
}
