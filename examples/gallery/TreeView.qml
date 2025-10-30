// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import Qt.labs.qmlmodels

Kirigami.Page {
    title: "TreeView QtQuick.Controls"

    Kirigami.ColumnView.interactiveResizeEnabled: true
    Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 10
    Kirigami.ColumnView.preferredWidth: Kirigami.Units.gridUnit * 25
    Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 50

    Item {
        anchors.fill: parent

        Controls.HorizontalHeaderView {
            id: horizontalHeader
            anchors.left: tableView.left
            anchors.top: parent.top
            syncView: tableView
            clip: true
            textRole: "display"
        }

        Controls.VerticalHeaderView {
            id: verticalHeader
            anchors.top: tableView.top
            anchors.left: parent.left
            syncView: tableView
            clip: true
            textRole: "display"
        }
        TreeView {
            id: treeView
            anchors.fill: parent

            selectionModel: ItemSelectionModel {}

            model: TreeModel {
                id: treeModel

                TableModelColumn {
                    display: "checked"
                }
                TableModelColumn {
                    display: "size"
                }
                TableModelColumn {
                    display: "type"
                }
                TableModelColumn {
                    display: "name"
                }
                TableModelColumn {
                    display: "lastModified"
                }

                rows: [
                    {
                        checked: false,
                        size: "—",
                        type: "folder",
                        name: "Documents",
                        lastModified: "2025-07-01",
                        rows: [
                            {
                                checked: true,
                                size: "24 KB",
                                type: "file",
                                name: "Resume.pdf",
                                lastModified: "2025-06-20"
                            },
                            {
                                checked: false,
                                size: "2 MB",
                                type: "folder",
                                name: "Reports",
                                lastModified: "2025-06-10",
                                rows: [
                                    {
                                        checked: true,
                                        size: "850 KB",
                                        type: "file",
                                        name: "Q2_Report.docx",
                                        lastModified: "2025-06-15"
                                    },
                                    {
                                        checked: false,
                                        size: "1.2 MB",
                                        type: "file",
                                        name: "Q3_Plan.xlsx",
                                        lastModified: "2025-06-18"
                                    }
                                ]
                            }
                        ]
                    },
                    {
                        checked: false,
                        size: "—",
                        type: "folder",
                        name: "Pictures",
                        lastModified: "2025-05-30",
                        rows: [
                            {
                                checked: true,
                                size: "3.5 MB",
                                type: "file",
                                name: "Vacation.jpg",
                                lastModified: "2025-05-15"
                            },
                            {
                                checked: false,
                                size: "2.1 MB",
                                type: "file",
                                name: "Family.png",
                                lastModified: "2025-05-20"
                            }
                        ]
                    }
                ]
            }

            delegate: Controls.TreeViewDelegate {}
        }
    }
}
