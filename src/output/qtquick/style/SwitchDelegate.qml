/* SPDX-FileCopyrightText: 2025 Niccolò Venerandi <niccolo@venerandi.com>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

import "private" as P

T.SwitchDelegate {
    id: control

    property bool __alternatingColors: (TableView.view?.alternatingRows && row % 2) || (Union.OutputProperties.useAlternatingColors && index % 2)

    Union.Element.type: "SwitchDelegate"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.down
        checked: control.checked
        enabled: control.enabled
        highlighted: control.highlighted
    }
    Union.Element.hints: [
        Union.ElementHint { name: "with-icon"; when: control.icon.name || control.icon.source.toString() },
        Union.ElementHint { name: "hover-enabled"; when: control.hoverEnabled },
        Union.ElementHint { name: "inside-list"; when: control.ListView?.view },
        Union.ElementHint { name: "inside-table"; when: control.TableView?.view },
        Union.ElementHint { name: "alternating-colors"; when: control.__alternatingColors },
    ]

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Union.Positioner.implicitWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Union.Positioner.implicitHeight)

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding: Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    spacing: Union.Style.properties.layout.spacing

    icon {
        color: palette.buttonText
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
        source: Union.Style.properties.icon.source
    }

    Union.Positioner.positionItems: [indicator, contentItem]

    contentItem: P.DefaultContentItem { control: control }

    indicator:  Item {
        Union.Element.type: "Indicator"
        Union.Element.states {
            hovered: control.hovered
            activeFocus: control.activeFocus
            visualFocus: control.visualFocus
            pressed: control.down
            checked: control.checked
            enabled: control.enabled
        }

        implicitWidth: Union.Style.properties.layout.width ?? 0
        implicitHeight: Union.Style.properties.layout.height ?? 0

        Union.StyledRectangle { anchors.fill: parent }

        Union.StyledRectangle {
            Union.Element.type: "Handle"
            Union.Element.states {
                hovered: control.hovered
                activeFocus: control.activeFocus
                visualFocus: control.visualFocus
                pressed: control.down
                checked: control.checked
                enabled: control.enabled
            }

            anchors.verticalCenter: parent.verticalCenter

            property real minimumX: Union.Style.properties.layout.margins.left ?? 0
            property real maximumX: parent.width - width - (Union.Style.properties.layout.margins.right ?? 0)
            x: Math.max(minimumX, Math.min(maximumX, minimumX + control.visualPosition * (maximumX - minimumX)))
        }
    }

    background: Union.StyledRectangle { }
}
