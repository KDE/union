// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami

Kirigami.ApplicationWindow {
    id: application

    title: "Control Gallery"

    width: pagePatterns.length * pageStack.defaultColumnWidth + globalDrawer.width + Kirigami.Units.gridUnit
    height: width * (9/16)

    property var pagePatterns: [
        "%1_org_kde_union.qml",
        "%1_org_kde_breeze.qml",
        "%1_org_kde_desktop.qml",
        "%1_org_kde_plasma_components.qml"
    ]

    function pushPage(page) {
        pageStack.clear()
        for (let pattern of pagePatterns) {
            try {
                pageStack.push(Qt.resolvedUrl(pattern.arg(page)))
            } catch (error) {
                print(error)
                continue
            }
        }
        pageStack.currentIndex = 0
    }

    pageStack.defaultColumnWidth: Kirigami.Units.gridUnit * 25
    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.DynamicColumns

    globalDrawer: Kirigami.GlobalDrawer {
        width: Kirigami.Units.gridUnit * 15
        modal: false

        header: ToolBar { }

        actions: [
            Kirigami.Action {
                text: "Buttons"
                onTriggered: application.pushPage("Buttons")
            },
            Kirigami.Action {
                text: "Text"
                onTriggered: application.pushPage("Text")
            },
            Kirigami.Action {
                text: "Checkable Controls"
                onTriggered: application.pushPage("Checkable")
            },
            Kirigami.Action {
                text: "Delegates"
                onTriggered: application.pushPage("Delegates")
            },
            Kirigami.Action {
                text: "Progress Bar & Slider"
                onTriggered: application.pushPage("ProgressSlider")
            },
            Kirigami.Action {
                text: "Dialogs"
                onTriggered: application.pushPage("Dialogs")
            },
            Kirigami.Action {
                text: "Menus"
                onTriggered: application.pushPage("Menus")
            },
            Kirigami.Action {
                text: "Tables"
                onTriggered: application.pushPage("Tables")
            },
            Kirigami.Action {
                text: "Tabs"
                onTriggered: application.pushPage("Tabs")
            },
            Kirigami.Action {
                text: "TreeView"
                onTriggered: application.pushPage("TreeView")
            },
            Kirigami.Action {
                text: "SwipeView"
                onTriggered: application.pushPage("SwipeView")
            }
        ]
    }

    pageStack.initialPage: Kirigami.Page {
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            icon.name: "breeze-settings"
            text: "Control Gallery"
            explanation: "Select an item in the sidebar to show controls."
        }
    }
}
