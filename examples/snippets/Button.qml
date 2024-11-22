import QtQuick
import QtQuick.Layouts

import QtTest

import org.kde.union

import org.kde.plasma.components as PC

ApplicationWindow {
    width: 800
    height: 600
    visible: true

    function cycleButtonDisplay(button) {
        switch (button.display) {
            case Button.TextBesideIcon:
                button.display = Button.IconOnly
                break
            case Button.IconOnly:
                button.display = Button.TextOnly
                break;
            case Button.TextOnly:
                button.display = Button.TextUnderIcon
                break
            case Button.TextUnderIcon:
                button.display = Button.TextBesideIcon
                break
        }
    }

    header: ToolBar {
        Element.elementId: "window-header"

        RowLayout {
            ToolButton {
                id: toolButton1
                text: "Header ToolButton 1"
                icon.name: "document-save"
                checkable: true
            }
            ToolButton {
                text: "Header ToolButton 2"
                icon.name: "document-save"
                enabled: !toolButton1.checked
            }
        }
    }

    GridLayout {
        Element.elementId: "window-contents"

        anchors.centerIn: parent

        columns: 2

        Button {
            id: button1
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            icon.name: "document-save"
            text: "Test"
            checkable: true
            onClicked: cycleButtonDisplay(this)
        }

        PC.Button {
            id: pcButton1
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            icon.name: "document-save"
            text: "Test"
            checkable: true
            onClicked: cycleButtonDisplay(this)
        }

        Button {
            text: "Test 2"
            icon.name: "document-save"
            enabled: !button1.checked
            onClicked: cycleButtonDisplay(this)
        }

        PC.Button {
            text: "Test 2"
            icon.name: "document-save"
            enabled: !pcButton1.checked
            onClicked: cycleButtonDisplay(this)
        }

        SpinBox {
        }

        PC.SpinBox {
        }

        ItemDelegate {
            icon.name: "document-save"
            text: "Test 4"
            onClicked: cycleButtonDisplay(this)
        }

        PC.ItemDelegate {
            icon.name: "document-save"
            text: "Test 4"
            onClicked: cycleButtonDisplay(this)
        }

        CheckBox {
            icon.name: "document-save"
            text: "Test 5"
            onClicked: cycleButtonDisplay(this)
        }

        PC.CheckBox {
            icon.name: "document-save"
            text: "Test 5"
            onClicked: cycleButtonDisplay(this)
        }

        CheckDelegate {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            icon.name: "document-save"
            text: "Test 5"
            onClicked: cycleButtonDisplay(this)
        }

        PC.CheckDelegate {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            icon.name: "document-save"
            text: "Test 5"
            onClicked: cycleButtonDisplay(this)
        }
    }

    footer: ToolBar {
        Element.elementId: "window-footer"

        enabled: !toolButton1.checked

        TextField {
            anchors.fill: parent

            placeholderText: "Placeholder Text"
        }
    }
}

