// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Kirigami.Page {
    title: "Text QtQuick.Controls"

    Kirigami.ColumnView.interactiveResizeEnabled: true
    Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 10
    Kirigami.ColumnView.preferredWidth: Kirigami.Units.gridUnit * 25
    Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 50

    ColumnLayout {
        Controls.Label {
            text: "Label"
        }

        Kirigami.Heading {
            level: 1
            text: "Heading"
        }

        Controls.TextField {
            placeholderText: "Text Field"
        }

        Controls.TextArea {
            placeholderText: "Text Area"
        }

        Controls.SpinBox {
            from: 0
            to: 1000
            stepSize: 1
        }
    }
}
