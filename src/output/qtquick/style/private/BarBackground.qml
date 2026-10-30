// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

Item {
    id: root

    required property T.Control control
    required property bool horizontal
    required property real from
    required property real to
    required property real startPosition
    required property real endPosition

    required property real tickMarkStepSize

    property real handleWidth
    property real handleHeight

    property string filledSubElementName: "Filled"

    readonly property real range: to - from
    readonly property real visualStartPosition: horizontal ? (control.mirrored ? 1.0 - endPosition : startPosition) : 1.0 - endPosition

    readonly property int tickMarkCount: tickMarkStepSize > 0 ? Math.round(range / tickMarkStepSize) + 1 : 0
    readonly property real tickMarkSpacing: tickMarkCount > 1 ? (horizontal ? (width - handleWidth) / (tickMarkCount - 1) : (height - handleHeight) / (tickMarkCount - 1)) : 0

    readonly property real filledSize: (root.endPosition - root.startPosition) * (horizontal ? width - handleWidth : height - handleHeight)

    implicitWidth: Union.Style.properties.layout.width + (horizontal ? 0 : tickMarkArea.width)
    implicitHeight: Union.Style.properties.layout.height + (horizontal ? tickMarkArea.height : 0)

    Union.StyledRectangle {
        id: track

        x: root.horizontal ? 0 : (parent.width - width) / 2
        y: !root.horizontal ? 0 : (parent.height - height) / 2
        width: root.horizontal ? parent.width : Union.Style.properties.layout.width
        height: root.horizontal ? Union.Style.properties.layout.height : parent.height
    }

    Union.StyledRectangle {
        Union.Element.subElement: root.filledSubElementName

        x: root.horizontal ? root.visualStartPosition * (parent.width - root.handleWidth - (root.endPosition < 0 ? width : 0)) : track.x
        y: root.horizontal ? track.y : root.visualStartPosition * (parent.height - root.handleHeight)
        width: root.horizontal ? (root.endPosition >= 0 ? root.filledSize + root.handleWidth : Union.Style.properties.layout.width) : track.width
        height: root.horizontal ? track.height : (root.endPosition >= 0 ? root.filledSize + root.handleHeight : Union.Style.properties.layout.height)
    }

    // TODO: Creating a bunch of items just to render some small lines is not
    // very efficient. We should turn this into a more proper styled item.
    Item {
        id: tickMarkArea

        Union.Element.subElement: "TickMark"

        x: root.horizontal ? root.handleWidth / 2 : track.x + (track.width / 2 - width / 2) + (root.control.mirrored ? -1.0 : 1.0) * (Union.Style.properties.layout.margins.left ?? 0)
        y: root.horizontal ? track.y + (track.height / 2 - height / 2) + (Union.Style.properties.layout.margins.top ?? 0) : root.handleHeight / 2

        width: root.horizontal ? parent.width - root.handleWidth : Union.Style.properties.layout.width
        height: root.horizontal ? Union.Style.properties.layout.height : parent.height - root.handleHeight

        visible: root.tickMarkCount > 0

        Repeater {
            model: root.tickMarkCount

            Union.StyledRectangle {
                id: tick

                required property int index

                readonly property real value: root.from + root.tickMarkStepSize * index
                readonly property real position: value / root.range
                property bool highlighted: root.startPosition <= position && root.endPosition >= position

                style: highlighted ? tickMarkHighlight.Union.Style : tickMarkArea.Union.Style

                x: root.horizontal ? (root.control.mirrored ? 1.0 - position : position) * parent.width - width / 2 : 0
                y: root.horizontal ? 0 : parent.height - (position * parent.height - height / 2)
            }
        }
    }

    Item {
        // Helper to query the highlighted style for tickmarks so we don't have
        // to do this for each individual tick mark.
        id: tickMarkHighlight
        Union.Element.subElement: "TickMark"
        Union.Element.states.highlighted: true
    }
}
