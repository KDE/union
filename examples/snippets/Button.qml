import QtQuick
import QtQuick.Layouts

import QtTest

import org.kde.union

import org.kde.plasma.components as PC

ApplicationWindow {
    width: 800
    height: 600
    visible: true

    header: ToolBar {
        Element.elementId: "window-header"

        RowLayout {
            ToolButton {
                text: "Header ToolButton 1"
                icon.name: "document-save"
            }
            ToolButton {
                text: "Header ToolButton 2"
                icon.name: "document-save"
            }
        }
    }

    GridLayout {
        Element.elementId: "window-contents"

        anchors.centerIn: parent

        columns: 2

        Button {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            icon.name: "document-save"
            text: "Test"
        }

        PC.Button {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            icon.name: "document-save"
            text: "Test"
        }

        Button {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            text: "Test 2"
        }

        PC.Button {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            text: "Test 2"
        }
    }

    footer: ToolBar {
        Element.elementId: "window-footer"

        TextField {
            anchors.fill: parent
        }
    }
}

