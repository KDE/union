// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Layouts

import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

Kirigami.Page {
    title: "Checkable Controls QtQuick.Controls"

    Kirigami.ColumnView.interactiveResizeEnabled: true
    Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 10
    Kirigami.ColumnView.preferredWidth: Kirigami.Units.gridUnit * 25
    Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 50

    ColumnLayout {
        anchors.fill: parent

        Controls.CheckBox {
            text: "Checkbox with Text"
        }

        Controls.CheckBox {
            icon.name: "document-save"
            text: "Checkbox with Text and Icon"
        }

        // Controls.RadioButton {
        //     text: "RadioButton with Text"
        // }

        // Controls.RadioButton {
        //     icon.name: "document-save"
        //     text: "RadioButton withh Text and Icon"
        // }

        Controls.Switch {
            text: "Switch with Text"
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
