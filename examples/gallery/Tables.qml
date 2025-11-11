// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import Qt.labs.qmlmodels

Kirigami.Page {
    title: "Tables QtQuick.Controls"

    Kirigami.ColumnView.interactiveResizeEnabled: true
    Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 10
    Kirigami.ColumnView.preferredWidth: Kirigami.Units.gridUnit * 25
    Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 50

    ColumnLayout {
        anchors.fill: parent

        Controls.CheckBox {
            text: "Tableview enabled"
            checked: true
            onCheckedChanged: {
                tableView.enabled = checked;
                horizontalHeader.enabled = checked;
                verticalHeader.enabled = checked;
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
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

            TableView {
                id: tableView
                anchors.left: verticalHeader.right
                anchors.top: horizontalHeader.bottom
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                clip: true
                alternatingRows: true
                acceptedButtons: Qt.NoButton

                model: TableModel {
                    TableModelColumn {
                        display: "name"
                    }
                    TableModelColumn {
                        display: "color"
                    }
                    TableModelColumn {
                        display: "testing"
                    }

                    rows: [
                        {
                            "name": "cat",
                            "color": "black",
                            "testing": "A"
                        },
                        {
                            "name": "dog",
                            "color": "brownnnnnnnnnnnnnnnnnnnnnnnnnn",
                            "testing": "B"
                        },
                        {
                            "name": "bird",
                            "color": "white",
                            "testing": "CCCCCCCCCCCC"
                        },
                        {
                            "name": "lizard",
                            "color": "green",
                            "testing": "DDDDDD"
                        },
                        {
                            "name": "fishiesssssssss",
                            "color": "blue",
                            "testing": "EEEEE"
                        }
                    ]
                }

                selectionModel: ItemSelectionModel {
                    model: tableView.model
                }

                delegate: Controls.TableViewDelegate {}
            }

            Controls.SelectionRectangle {
                target: tableView
            }
        }
    }
}
