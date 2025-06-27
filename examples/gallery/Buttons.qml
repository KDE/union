import QtQuick
import QtQuick.Layouts

import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

Kirigami.Page {
    title: "Buttons QtQuick.Controls"

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
            text: "ToolButton Checkable"
            checkable: true
            checked: true
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
