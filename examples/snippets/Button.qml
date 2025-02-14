/*
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>*
 */

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
        Element.elementId: "application-header"

        RowLayout {
            anchors.fill: parent

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

            ToolSeparator { }

            PC.ToolButton {
                text: "PC3 ToolButton"
                icon.name: "document-save"

                onClicked: pc3menu.popup(this, 0, height)

                PC.Menu {
                    id: pc3menu

                    PC.MenuItem { text: "Example item" }
                    PC.Menu {
                        title: "Example Submenu"

                        PC.MenuItem { text: "Example Submenu Item 1" }
                        PC.MenuItem { text: "Example Submenu Item 2" }
                    }
                    PC.MenuItem {
                        text: "Checkable Item";
                        checkable: true;
                    }
                    PC.MenuSeparator { }
                    PC.MenuItem {
                        text: "Quit";
                        icon.name: "application-exit-symbolic"
                        onClicked: Qt.quit()
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            ToolButton {
                display: ToolButton.IconOnly
                icon.name: "application-menu-symbolic"

                checkable: true
                checked: menu.opened

                onClicked: menu.popup(this, 0, height)

                Menu {
                    id: menu

                    MenuItem { text: "Example item" }
                    Menu {
                        title: "Example Submenu"

                        MenuItem { text: "Example Submenu Item 1" }
                        MenuItem { text: "Example Submenu Item 2" }
                    }
                    MenuItem {
                        text: "Checkable Item";
                        checkable: true
                        onClicked: checked = !checked
                    }
                    MenuSeparator { }
                    MenuItem {
                        text: "Quit";
                        icon.name: "application-exit-symbolic"
                        onClicked: Qt.quit()
                    }
                }
            }
        }
    }

    ScrollView {
        anchors.fill: parent

        // contentWidth: layout.implicitWidth
        // contentHeight: layout.implicitHeight

        GridLayout {
            id: layout
            Element.elementId: "window-contents"

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
                enabled: !button1.checked
            }

            PC.SpinBox {
                enabled: !pcButton1.checked
            }

            ItemDelegate {
                icon.name: "document-save"
                text: "Test 4"
                enabled: !button1.checked
                onClicked: cycleButtonDisplay(this)
            }

            PC.ItemDelegate {
                icon.name: "document-save"
                text: "Test 4"
                enabled: !pcButton1.checked
                onClicked: cycleButtonDisplay(this)
            }

            CheckBox {
                icon.name: "document-save"
                text: "Test 5"
                enabled: !button1.checked
                onClicked: cycleButtonDisplay(this)
            }

            PC.CheckBox {
                icon.name: "document-save"
                text: "Test 5"
                enabled: !pcButton1.checked
                onClicked: cycleButtonDisplay(this)
            }

            CheckDelegate {
                Layout.preferredWidth: 200
                Layout.preferredHeight: 50
                icon.name: "document-save"
                text: "Test 5"
                enabled: !button1.checked
                onClicked: cycleButtonDisplay(this)
            }

            PC.CheckDelegate {
                Layout.preferredWidth: 200
                Layout.preferredHeight: 50
                icon.name: "document-save"
                text: "Test 5"
                enabled: !pcButton1.checked
                onClicked: cycleButtonDisplay(this)
            }

            Switch {
                text: "Test Switch"
            }

            PC.Switch {
                text: "Test Switch"
            }
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

