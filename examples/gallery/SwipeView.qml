// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import Qt.labs.qmlmodels

Kirigami.Page {
    title: "SwipeView QtQuick.Controls"

    Kirigami.ColumnView.interactiveResizeEnabled: true
    Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 10
    Kirigami.ColumnView.preferredWidth: Kirigami.Units.gridUnit * 25
    Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 50

    Item {
        anchors.fill: parent

        Controls.SwipeView {
            id: view
            currentIndex: pageIndicator.currentIndex
            anchors.fill: parent

            Controls.Page {
                title: "AAAA"
                Controls.Label {
                    anchors.centerIn: parent
                    text: "Page 1"
                }
            }
            Controls.Page {
                title: "BBBB"
                Controls.Label {
                    anchors.centerIn: parent
                    text: "Page 2"
                }
            }
            Controls.Page {
                title: "CCCC"
                Controls.Label {
                    anchors.centerIn: parent
                    text: "Page 3"
                }
            }
        }

        Controls.PageIndicator {
            id: pageIndicator
            interactive: true
            count: view.count
            currentIndex: view.currentIndex
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}



