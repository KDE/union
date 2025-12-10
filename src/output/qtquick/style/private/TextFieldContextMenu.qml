/*
 *    SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
 *    SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
 *    SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
 *    SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Templates as T
import org.kde.union.impl as Union

Menu {
    id: root

    property Item target

    modal: true

    MenuItem {
        id: undoItem
        action: T.Action {
            icon.name: "edit-undo-symbolic"
            text: qsTr("Undo")
            shortcut: StandardKey.Undo
        }
        visible: root.target !== null && !root.target.readOnly && root.target.echoMode !== TextInput.PasswordEchoOnEdit && root.target.echoMode !== TextInput.Password
        enabled: root.target?.canUndo ?? false
        onTriggered: {
            root.target.undo();
        }
    }
    MenuItem {
        id: redoItem
        action: T.Action {
            icon.name: "edit-redo-symbolic"
            text: qsTr("Redo")
            shortcut: StandardKey.Redo
        }
        visible: undoItem.visible
        enabled: root.target?.canRedo ?? false
        onTriggered: {
            root.target.redo();
        }
    }
    MenuSeparator {
        visible: undoItem.visible || redoItem.visible
    }
    MenuItem {
        id: cutItem
        action: T.Action {
            icon.name: "edit-cut-symbolic"
            text: qsTr("Cut")
            shortcut: StandardKey.Cut
        }
        visible: root.target !== null && !root.target.readOnly && root.target.echoMode !== TextInput.PasswordEchoOnEdit && root.target.echoMode !== TextInput.Password
        enabled: root.target.selectedText !== ""
        onTriggered: {
            root.target.cut();
        }
    }
    MenuItem {
        id: copyItem
        action: T.Action {
            icon.name: "edit-copy-symbolic"
            text: qsTr("Copy")
            shortcut: StandardKey.Copy
        }
        visible: root.target !== null && root.target.echoMode !== TextInput.PasswordEchoOnEdit && root.target.echoMode !== TextInput.Password
        enabled: root.target.selectedText !== ""
        onTriggered: {
            root.target.copy();
        }
    }
    MenuItem {
        id: pasteItem
        action: T.Action {
            icon.name: "edit-paste-symbolic"
            text: qsTr("Paste")
            shortcut: StandardKey.Paste
        }
        visible: copyItem.visible
        enabled: root.target?.canPaste ?? false
        onTriggered: {
            root.target.paste();
        }
    }
    MenuItem {
        id: deleteItem
        action: T.Action {
            icon.name: "edit-delete-symbolic"
            text: qsTr("Delete")
            shortcut: StandardKey.Delete
        }
        visible: cutItem.visible
        enabled: root.target.selectedText !== ""
        onTriggered: {
            root.target.remove(root.target.selectionStart, root.target.selectionEnd);
        }
    }
    MenuSeparator {
        visible: cutItem.visible || copyItem.visible || pasteItem.visible || deleteItem.visible
    }
    MenuItem {
        id: selectAllItem
        action: T.Action {
            icon.name: "edit-select-all-symbolic"
            text: qsTr("Select All")
            shortcut: StandardKey.SelectAll
        }
        visible: root.target !== null
        onTriggered: {
            root.target.selectAll();
        }
    }
}
