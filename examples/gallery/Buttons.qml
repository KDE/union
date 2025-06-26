// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Layouts

import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

Kirigami.Page {
    title: "Buttons QtQuick.Controls"

    ColumnLayout {
        anchors.fill: parent

        Controls.Button {
            text: "Button with Text"
        }

        Controls.Button {
            icon.name: "document-save"
            text: "Button with Text and Icon"
        }

        Controls.Button {
            icon.name: "document-save"
            text: "Flat"
            flat: true
        }

        Controls.Button {
            icon.name: "document-save"
            text: "Text Under Icon"
            display: Controls.Button.TextUnderIcon
        }

        Controls.Button {
            icon.name: "document-save"
            text: "Checkable"
            checkable: true
            checked: true
        }

        Controls.Button {
            icon.name: "document-save"
            text: "Highlighted"
            highlighted: true
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
