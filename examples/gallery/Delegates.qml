// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Layouts

import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

Kirigami.Page {
    title: "Delegates QtQuick.Controls"

    Kirigami.ColumnView.interactiveResizeEnabled: true
    Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 10
    Kirigami.ColumnView.preferredWidth: Kirigami.Units.gridUnit * 25
    Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 50

    ColumnLayout {
        anchors.fill: parent

        Controls.ItemDelegate {
            text: "ItemDelegate with Text"
        }

        Controls.ItemDelegate {
            icon.name: "document-save"
            text: "ItemDelegate with Text and Icon"
        }

        Controls.ItemDelegate {
            icon.name: "document-save"
            text: "ItemDelegate Highlighted"
            highlighted: true
        }

        Controls.CheckDelegate {
            text: "CheckDelegate with Text"
        }

        Controls.RadioDelegate {
            text: "RadioDelegate n. 1 with Text"
        }

        Controls.RadioDelegate {
            text: "RadioDelegate n. 2 with Text"
        }

        Controls.SwitchDelegate {
            text: "SwitchDelegate with Text"
        }

        Controls.SwipeDelegate {
            text: "Swipe me!"

            swipe.right: Rectangle {
                anchors.fill: parent
                color: "blue"
            }
        }

        Controls.Slider {
            from: 0
            to: 100
            stepSize: 1
            value: 50
        }

        Controls.Slider {
            from: 0
            to: 100
            stepSize: 1
            value: 50
            orientation: Qt.Vertical
        }

        Controls.RangeSlider {
            from: 0
            to: 100
            stepSize: 1
            first.value: 25
            second.value: 75
        }

        Controls.RangeSlider {
            from: 0
            to: 100
            stepSize: 1
            first.value: 25
            second.value: 75
            orientation: Qt.Vertical
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
