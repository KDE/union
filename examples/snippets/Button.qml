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
            id: button1
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            icon.name: "document-save"
            text: "Test"
            checkable: true
        }

        PC.Button {
            id: pcButton1
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            icon.name: "document-save"
            text: "Test"
            checkable: true
        }

        Button {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            text: "Test 2"
            icon.name: "document-save"
            enabled: !button1.checked

            display: Button.TextUnderIcon

            onClicked: {
                switch (display) {
                    case Button.TextBesideIcon:
                        display = Button.IconOnly
                        break
                    case Button.IconOnly:
                        display = Button.TextOnly
                        break;
                    case Button.TextOnly:
                        display = Button.TextUnderIcon
                        break
                    case Button.TextUnderIcon:
                        display = Button.TextBesideIcon
                        break
                }
            }
        }

        PC.Button {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            text: "Test 2"
            icon.name: "document-save"
            enabled: !pcButton1.checked

            onClicked: {
                switch (display) {
                    case Button.TextBesideIcon:
                        display = Button.IconOnly
                        break
                    case Button.IconOnly:
                        display = Button.TextOnly
                        break;
                    case Button.TextOnly:
                        display = Button.TextUnderIcon
                        break
                    case Button.TextUnderIcon:
                        display = Button.TextBesideIcon
                        break
                }
            }
        }

        SpinBox {
        }

        PC.SpinBox {
        }

        ItemDelegate {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            icon.name: "document-save"
            text: "Test 4"
            onClicked: {
                switch (display) {
                    case Button.TextBesideIcon:
                        display = Button.IconOnly
                        break
                    case Button.IconOnly:
                        display = Button.TextOnly
                        break;
                    case Button.TextOnly:
                        display = Button.TextUnderIcon
                        break
                    case Button.TextUnderIcon:
                        display = Button.TextBesideIcon
                        break
                }
            }
        }

        PC.ItemDelegate {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 50
            icon.name: "document-save"
            text: "Test 4"
        }
    }

    footer: ToolBar {
        Element.elementId: "window-footer"

        TextField {
            anchors.fill: parent
        }
    }
}

