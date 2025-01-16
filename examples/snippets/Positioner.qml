/*
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>*
 */

import QtQuick
import QtQuick.Layouts

import org.kde.union as Union

Union.ApplicationWindow {
    width: 1024
    height: 600
    visible: true

    GridLayout {
        columnSpacing: 4
        rowSpacing: 4

        Repeater {
            model: [
                {
                    row: 0,
                    column: 0,
                    text: "H: Start\nV: Center",
                    horizontal: [Union.Alignment.Start, Union.Alignment.Start, Union.Alignment.Start],
                    vertical: [Union.Alignment.Center, Union.Alignment.Center, Union.Alignment.Center]
                },
                {
                    row: 0,
                    column: 1,
                    text: "H: Start, Center, End\nV: Center",
                    horizontal: [Union.Alignment.Start, Union.Alignment.Center, Union.Alignment.End],
                    vertical: [Union.Alignment.Center, Union.Alignment.Center, Union.Alignment.Center]
                },
                {
                    row: 0,
                    column: 2,
                    text: "H: Start, Fill, End\nV: Center",
                    horizontal: [Union.Alignment.Start, Union.Alignment.Fill, Union.Alignment.End],
                    vertical: [Union.Alignment.Center, Union.Alignment.Center, Union.Alignment.Center]
                },
                {
                    row: 0,
                    column: 3,
                    text: "H: Start, Start, Fill\nV: Center",
                    horizontal: [Union.Alignment.Start, Union.Alignment.Start, Union.Alignment.Fill],
                    vertical: [Union.Alignment.Center, Union.Alignment.Center, Union.Alignment.Center]
                },
                {
                    row: 0,
                    column: 4,
                    text: "H: Start, Center, Start\nV: Center",
                    horizontal: [Union.Alignment.Start, Union.Alignment.Center, Union.Alignment.Start],
                    vertical: [Union.Alignment.Center, Union.Alignment.Center, Union.Alignment.Center]
                },
                {
                    row: 1,
                    column: 0,
                    text: "H: Start\nV: Start, Center, End",
                    horizontal: [Union.Alignment.Start, Union.Alignment.Start, Union.Alignment.Start],
                    vertical: [Union.Alignment.Start, Union.Alignment.Center, Union.Alignment.End]
                },
                {
                    row: 1,
                    column: 1,
                    text: "H: Start, Fill, End\nV: Fill, Center, End",
                    horizontal: [Union.Alignment.Start, Union.Alignment.Fill, Union.Alignment.End],
                    vertical: [Union.Alignment.Fill, Union.Alignment.Center, Union.Alignment.End]
                },
                {
                    row: 1,
                    column: 2,
                    text: "H: Fill\nV: Fill",
                    horizontal: [Union.Alignment.Fill, Union.Alignment.Fill, Union.Alignment.Fill],
                    vertical: [Union.Alignment.Fill, Union.Alignment.Fill, Union.Alignment.Fill]
                },
                {
                    row: 1,
                    column: 3,
                    text: "H: Fill, End, End\nV: Center, Stack, Stack",
                    horizontal: [Union.Alignment.Fill, Union.Alignment.End, Union.Alignment.End],
                    vertical: [Union.Alignment.Center, Union.Alignment.Stack, Union.Alignment.Stack]
                },
            ]

            Rectangle {
                implicitWidth: 200
                implicitHeight: 200
                color: Qt.rgba(0.97, 0.97, 0.97, 1.0)

                Layout.row: modelData.row
                Layout.column: modelData.column

                Union.Positioner.positionItems: [horizontalRed, horizontalGreen, horizontalBlue]

                Rectangle {
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: childrenRect.width
                    height: childrenRect.height
                    z: 1
                    color: "white"
                    Text {
                        text: modelData.text
                    }
                }

                Rectangle {
                    id: horizontalRed
                    implicitWidth: 25
                    implicitHeight: 25
                    color: "red"

                    Union.Style.properties.layout.alignment {
                        horizontal: modelData.horizontal[0]
                        vertical: modelData.vertical[0]
                        order: 0
                    }
                }

                Rectangle {
                    id: horizontalGreen
                    implicitWidth: 50
                    implicitHeight: 30
                    color: "green"

                    Union.Style.properties.layout.alignment {
                        horizontal: modelData.horizontal[1]
                        vertical: modelData.vertical[1]
                        order: 1
                    }
                }

                Rectangle {
                    id: horizontalBlue
                    implicitWidth: 25
                    implicitHeight: 25
                    color: "blue"

                    Union.Style.properties.layout.alignment {
                        horizontal: modelData.horizontal[2]
                        vertical: modelData.vertical[2]
                        order: 2
                    }
                }
            }
        }
    }
}
