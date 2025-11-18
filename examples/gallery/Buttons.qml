// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Layouts

import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

Kirigami.Page {
    title: "Buttons QtQuick.Controls"

    Kirigami.ColumnView.interactiveResizeEnabled: true
    Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 10
    Kirigami.ColumnView.preferredWidth: Kirigami.Units.gridUnit * 25
    Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 50

    ColumnLayout {
        anchors.fill: parent

        Controls.Button {
            text: "Button with Text"
        }

        Controls.Button {
            icon.name: "document-save"
            text: "Button with Text and Icon"
        }

        Controls.Button {
            icon.name: "document-save"
            text: "Disabled Button"
            enabled: false
        }

        Controls.Button {
            icon.name: "document-save"
            text: "Button Flat"
            flat: true
        }

        Controls.Button {
            icon.name: "document-save"
            text: "Button Text Under Icon"
            display: Controls.Button.TextUnderIcon
        }

        Controls.Button {
            icon.name: "document-save"
            text: "Button Checkable"
            checkable: true
            checked: true
        }

        Controls.Button {
            icon.name: "document-save"
            text: "Button Highlighted"
            highlighted: true
        }

        Controls.ToolButton {
            text: "ToolButton with Text"
        }

        Controls.ToolButton {
            icon.name: "document-save"
            text: "ToolButton with Text and Icon"
        }

        Controls.ToolButton {
            icon.name: "document-save"
            text: "ToolButton Text Under Icon"
            display: Controls.Button.TextUnderIcon
        }

        Controls.ToolButton {
            icon.name: "document-save"
            text: "ToolButton Checkable"
            checkable: true
            checked: true
        }

        Controls.RoundButton {
            icon.name: "document-save"
            text: "RoundButton"
        }

        Controls.ComboBox {
            currentIndex: 1
            displayText: "Combobox: " + currentText
            model: ["AAAAAAAA", "BBBBBBBBBBB", "CCCCCCCCCCCCCC"]
        }

        Controls.ComboBox {
            enabled: false
            currentIndex: 1
            displayText: "Disabled Combobox: " + currentText
            model: ["I am not enabled!", "BBBBBBBBBBB", "CCCCCCCCCCCCCC"]
        }

        Controls.ComboBox {
            editable: true
            currentIndex: 1
            displayText: "Editable Combobox: " + currentText
            model: ["AAAAAAAA", "BBBBBBBBBBB", "CCCCCCCCCCCCCC"]
            selectTextByMouse: true
        }

        Controls.ComboBox {
            editable: true
            enabled: false
            currentIndex: 1
            displayText: "Disabled Editable Combobox: " + currentText
            model: ["I am not enabled!", "BBBBBBBBBBB", "CCCCCCCCCCCCCC"]
            selectTextByMouse: true
        }

        Controls.ComboBox {
            flat: true
            currentIndex: 1
            displayText: "Flat Combobox: " + currentText
            model: ["AAAAAAAA", "BBBBBBBBBBB", "CCCCCCCCCCCCCC"]
        }

        Controls.ComboBox {
            enabled: false
            flat: true
            currentIndex: 1
            displayText: "Disabled Flat Combobox: " + currentText
            model: ["I am not enabled!", "BBBBBBBBBBB", "CCCCCCCCCCCCCC"]
        }

        Controls.ComboBox {
            flat: true
            editable: true
            currentIndex: 1
            displayText: "Flat Editable Combobox: " + currentText
            model: ["AAAAAAAA", "BBBBBBBBBBB", "CCCCCCCCCCCCCC"]
            selectTextByMouse: true
        }

        Controls.ComboBox {
            flat: true
            editable: true
            enabled: false
            currentIndex: 1
            displayText: "Disabled Flat Editable Combobox: " + currentText
            model: ["I am not enabled!", "BBBBBBBBBBB", "CCCCCCCCCCCCCC"]
            selectTextByMouse: true
        }


        Item {
            Layout.fillHeight: true
        }
    }
}
