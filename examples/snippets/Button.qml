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
            }
            ToolButton {
                text: "Header ToolButton 2"
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
            text: "Test"
        }

        PC.Button {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
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

