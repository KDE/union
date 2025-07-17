// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Niccolò Venerandi <niccolo@venerandi.com>

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.DialogButtonBox {
    id: control

    Union.Element.type: "DialogButtonBox"

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    contentWidth: (contentItem as ListView)?.contentWidth ?? 0

    spacing: Union.Style.properties.layout.spacing
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom
    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right

    alignment: Qt.AlignRight

    Union.Element.attributes: {
        let result = {}
        switch (position) {
            case T.DialogButtonBox.Header:
                result.position = "header"
                break
            case T.DialogButtonBox.Footer:
                result.position = "footer"
                break
        }
        return result
    }

    delegate: Button {
        width: implicitWidth

        Union.Element.attributes: {
            let result = {}
            switch (DialogButtonBox.buttonRole) {
                case T.DialogButtonBox.InvalidRole:
                    result.role = "invalid"
                    break
                case T.DialogButtonBox.AcceptRole:
                    result.role = "accept"
                    break
                case T.DialogButtonBox.RejectRole:
                    result.role = "reject"
                    break
                case T.DialogButtonBox.DestructiveRole:
                    result.role = "destructive"
                    break
                case T.DialogButtonBox.ActionRole:
                    result.role = "action"
                    break
                case T.DialogButtonBox.HelpRole:
                    result.role = "help"
                    break
                case T.DialogButtonBox.YesRole:
                    result.role = "yes"
                    break
                case T.DialogButtonBox.NoRole:
                    result.role = "no"
                    break
                case T.DialogButtonBox.ResetRole:
                    result.role = "reset"
                    break
                case T.DialogButtonBox.ApplyRole:
                    result.role = "apply"
                    break
            }
            return result
        }
    }

    contentItem: ListView {
        pixelAligned: true
        model: control.contentModel
        spacing: control.spacing
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        snapMode: ListView.SnapToItem
    }

    background: Union.StyledRectangle { }

    // Standard buttons are destroyed and then recreated every time
    // the `standardButtons` property changes, so it is necessary to
    // run this code every time standardButtonsChanged() is emitted.
    // See QQuickDialogButtonBox::setStandardButtons()
    onStandardButtonsChanged: {
        // standardButton() returns a pointer to an existing standard button.
        // If no such button exists, it returns nullptr.
        // Icon names are copied from KStyle::standardIcon()
        function setStandardIcon(buttonType, iconName) {
            let button = standardButton(buttonType)
            // For some reason, `== ""` works, but `=== ""` and `!name && !source` doesn't.
            if (button && button.icon.name == "" && button.icon.source == "") {
                button.icon.name = iconName
            }
        }
        setStandardIcon(T.Dialog.Ok, "dialog-ok")
        setStandardIcon(T.Dialog.Save, "document-save")
        setStandardIcon(T.Dialog.SaveAll, "document-save-all")
        setStandardIcon(T.Dialog.Open, "document-open")
        setStandardIcon(T.Dialog.Yes, "dialog-ok-apply")
        setStandardIcon(T.Dialog.YesToAll, "dialog-ok")
        setStandardIcon(T.Dialog.No, "dialog-cancel")
        setStandardIcon(T.Dialog.NoToAll, "dialog-cancel")
        setStandardIcon(T.Dialog.Abort, "dialog-cancel")
        setStandardIcon(T.Dialog.Retry, "view-refresh")
        setStandardIcon(T.Dialog.Ignore, "dialog-cancel")
        setStandardIcon(T.Dialog.Close, "dialog-close")
        setStandardIcon(T.Dialog.Cancel, "dialog-cancel")
        setStandardIcon(T.Dialog.Discard, "edit-delete")
        setStandardIcon(T.Dialog.Help, "help-contents")
        setStandardIcon(T.Dialog.Apply, "dialog-ok-apply")
        setStandardIcon(T.Dialog.Reset, "edit-undo")
        setStandardIcon(T.Dialog.RestoreDefaults, "document-revert")
    }
}
