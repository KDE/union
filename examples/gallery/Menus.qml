// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Kirigami.Page {
    title: "Menu QtQuick.Controls"

    Kirigami.ColumnView.interactiveResizeEnabled: true
    Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 10
    Kirigami.ColumnView.preferredWidth: Kirigami.Units.gridUnit * 25
    Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 50

    ColumnLayout {
        anchors.fill: parent
        Controls.MenuBar {
            Controls.Menu {
                title: qsTr("File")
                Kirigami.Action {
                    text: qsTr("New...")
                }
                Kirigami.Action {
                    text: qsTr("Open...")
                }
                Controls.MenuSeparator {}
                Kirigami.Action {
                    icon.name: "document-save"
                    text: qsTr("Save")
                }
                Kirigami.Action {
                    text: qsTr("Save As...")
                }
            }
            Controls.Menu {
                title: qsTr("Edit")
                Kirigami.Action {
                    icon.name: "edit-cut"
                    text: qsTr("Cut")
                }
                Kirigami.Action {
                    icon.name: "edit-copy"
                    text: qsTr("Copy")
                }
                Kirigami.Action {
                    icon.name: "edit-paste"
                    text: qsTr("Paste")
                }
            }
            Controls.Menu {
                title: qsTr("Help")
                Kirigami.Action {
                    text: qsTr("About")
                }
            }
            Controls.Menu {
                title: qsTr("Disabled Menu")
                enabled: false
                Kirigami.Action {
                    text: qsTr("How did you open me??")
                    enabled: false
                }
            }
        }

        Controls.ToolBar {
            Layout.fillWidth: true

            RowLayout {
                Controls.ToolButton {
                    icon.name: "document-new"
                    text: "ToolButton with Menu"
                    Accessible.role: Accessible.ButtonMenu
                    // checkable: true
                    checked: toolbarMenu.visible

                    Controls.Menu {
                        id: toolbarMenu

                        Controls.MenuItem { text: "Menu Item 1" }
                        Controls.MenuItem { text: "Menu Item 2" }
                        Controls.MenuItem { text: "Menu Item 3" }
                    }

                    onClicked: toolbarMenu.popup(this, 0, height)
                }

                Controls.ToolSeparator {
                }

                Controls.ToolButton {
                    text: "Disabled ToolButton"
                    enabled: false
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
