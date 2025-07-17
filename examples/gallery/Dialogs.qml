// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Niccolò Venerandi <niccolo@venerandi.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Kirigami.Page {
    title: "Dialogs QtQuick.Controls"

    Kirigami.ColumnView.interactiveResizeEnabled: true
    Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 10
    Kirigami.ColumnView.preferredWidth: Kirigami.Units.gridUnit * 25
    Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 50

    ColumnLayout {
        anchors.fill: parent

        Controls.Button {
            text: "Show Dialog"
            onClicked: dialog.visible = true
        }

        Controls.Dialog {
            id: dialog
            title: "Title"
            standardButtons: Controls.Dialog.Ok | Controls.Dialog.Cancel
            visible: false

            onAccepted: console.log("Ok clicked")
            onRejected: console.log("Cancel clicked")
        }
    }
}
