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
    Union.Element.attributes: Union.ElementAttribute {
        name: "position"
        value: control.position === T.DialogButtonBox.Header ? "header" : "footer"
    }

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

    delegate: Button {
        width: implicitWidth

        Union.Element.attributes: Union.ElementAttribute {
            name: "role"
            value: switch (DialogButtonBox.buttonRole) {
                case T.DialogButtonBox.InvalidRole: return "invalid"
                case T.DialogButtonBox.AcceptRole: return "accept"
                case T.DialogButtonBox.RejectRole: return "reject"
                case T.DialogButtonBox.DestructiveRole: return "destructive"
                case T.DialogButtonBox.ActionRole: return "action"
                case T.DialogButtonBox.HelpRole: return "help"
                case T.DialogButtonBox.YesRole: return "yes"
                case T.DialogButtonBox.NoRole: return "no"
                case T.DialogButtonBox.ResetRole: return "reset"
                case T.DialogButtonBox.ApplyRole: return "apply"
            }
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
