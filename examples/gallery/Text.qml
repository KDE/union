// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Kirigami.Page {
    title: "Text QtQuick.Controls"

    Kirigami.ColumnView.interactiveResizeEnabled: true
    Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 10
    Kirigami.ColumnView.preferredWidth: Kirigami.Units.gridUnit * 25
    Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 50

    ColumnLayout {
        anchors.fill: parent

        Controls.Label {
            text: "Label"
        }

        Controls.Label {
            enabled: false
            text: "Disabled Label"
        }

        Kirigami.Heading {
            level: 1
            text: "Heading"
        }

        Kirigami.Heading {
            enabled: false
            level: 1
            text: "Disabled Heading"
        }

        Controls.TextField {
            placeholderText: "Text Field"
        }

        Controls.TextField {
            enabled: false
            placeholderText: "Disabled Text Field"
        }

        Kirigami.PasswordField {
            placeholderText: "Password field"
        }

        // Kirigami uses Application style, so this uses same style for all pages
        Kirigami.ActionTextField {
            id: actionTextField
            Layout.fillWidth: true
            placeholderText: "ActionTextField"
            Layout.preferredHeight: 40
            rightActions: [
                Kirigami.Action {
                    text: "Cancel"
                    icon.name: "edit-delete-remove"
                    onTriggered: {
                        console.warn("blah")
                    }
                },
                Kirigami.Action {
                    text: "Confirm"
                    icon.name: "checkmark"
                    visible: actionTextField.text.length > 0
                    onTriggered: {
                        console.warn("bluh")
                    }
                }
            ]
        }

        Controls.TextArea {
            placeholderText: "Text Area"
        }

        Controls.TextArea {
            enabled: false
            placeholderText: "Disabled Text Area"
        }

        Controls.ScrollView {
            Layout.fillWidth: true
            Layout.preferredHeight: 200

            Controls.TextArea {
                placeholderText: "Tell us more about the crash…"
                wrapMode: Text.Wrap
            }
        }

        Controls.SpinBox {
            from: 0
            to: 1000
            stepSize: 1
        }

        Controls.SpinBox {
            enabled: false
            from: 0
            to: 1000
            stepSize: 1
        }

        Controls.SpinBox {
            Layout.preferredWidth: 50
            from: 10
            to: 1000
            stepSize: 10
        }

        TryControl {
            name: "DoubleSpinBox"
            module: "QtQuick.Controls"

            properties: {
                "from": 0.0,
                "to": 1.0,
                "stepSize": 0.01
            }
        }

        SearchField {
            Layout.fillWidth: true
            module: "QtQuick.Controls"
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
