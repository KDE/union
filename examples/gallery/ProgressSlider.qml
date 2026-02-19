// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Layouts

import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

import org.kde.union as Union

Kirigami.Page {
    title: "Progress Bar & Slider QtQuick.Controls"

    Kirigami.ColumnView.interactiveResizeEnabled: true
    Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 10
    Kirigami.ColumnView.preferredWidth: Kirigami.Units.gridUnit * 25
    Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 50

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Controls.BusyIndicator {
                running: true
            }

            Controls.BusyIndicator {
                running: true
            }
        }

        Controls.ProgressBar {
            from: 0
            to: 100
            value: horizontalSlider.value
        }

        Controls.ProgressBar {
            from: 0
            to: 100
            value: horizontalSlider.value
            enabled: false
        }

        Controls.ProgressBar {
            indeterminate: true
        }

        Controls.ProgressBar {
            indeterminate: true
            enabled: false
        }

        Controls.Slider {
            id: horizontalSlider

            from: 0
            to: 100
            stepSize: 10
            value: 50
        }

        Controls.Slider {
            enabled: false
            from: 0
            to: 100
            value: horizontalSlider.value
        }

        Controls.Slider {
            from: 0
            to: 100
            stepSize: 10
            value: 50

            // Note that we're setting both stylehints here as we cannot be sure
            // the Union platform plugin for Kirigami is loaded so these may not
            // be forwarded to Union correctly.
            Union.StyleHints.tickMarkStepSize: 20
            Kirigami.StyleHints.tickMarkStepSize: 20
        }


        Controls.RangeSlider {
            id: horizontalRangeSlider
            from: 0
            to: 100
            stepSize: 1
            first.value: 25
            second.value: 75
        }

        Controls.RangeSlider {
            enabled: false
            from: 0
            to: 100
            stepSize: 1
            first.value: horizontalRangeSlider.first.value
            second.value: horizontalRangeSlider.second.value
        }

        RowLayout {
            Controls.Slider {
                from: 0
                to: 100
                stepSize: 10
                value: 50
                orientation: Qt.Vertical
            }

            Controls.Slider {
                enabled: false
                from: 0
                to: 100
                stepSize: 1
                value: 50
                orientation: Qt.Vertical
            }

            Controls.Slider {
                from: 0
                to: 100
                stepSize: 10
                value: 50
                orientation: Qt.Vertical

                Kirigami.StyleHints.tickMarkStepSize: 20
                Union.StyleHints.tickMarkStepSize: 20
            }

            Controls.RangeSlider {
                enabled: false
                from: 0
                to: 100
                stepSize: 1
                first.value: 25
                second.value: 75
                orientation: Qt.Vertical
            }

            Controls.RangeSlider {
                from: 0
                to: 100
                stepSize: 1
                first.value: 25
                second.value: 75
                orientation: Qt.Vertical
            }
        }

        Controls.ScrollBar {
            Layout.preferredHeight: 100
            orientation: Qt.Vertical
            size: 0.4
        }

        Item {
            Layout.fillHeight: true
        }
    }
}

