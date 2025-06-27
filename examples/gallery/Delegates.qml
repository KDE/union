// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Layouts

import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

Kirigami.Page {
    title: "Delegates QtQuick.Controls"

    ColumnLayout {
        anchors.fill: parent

        Controls.ItemDelegate {
            text: "ItemDelegate with Text"
        }

        Controls.ItemDelegate {
            icon.name: "document-save"
            text: "ItemDelegate with Text and Icon"
        }

        Controls.CheckDelegate {
            text: "CheckDelegate with Text"
        }

        // Controls.RadioDelegate {
        //     text: "RadioDelegate with Text"
        // }

        // Controls.SwitchDelegate {
        //     text: "SwitchDelegate with Text"
        // }

        Item {
            Layout.fillHeight: true
        }
    }
}

