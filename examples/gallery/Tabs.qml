// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Kirigami.Page {
    title: "Tabbar QtQuick.Controls"
    
    Kirigami.ColumnView.interactiveResizeEnabled: true
    Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 10
    Kirigami.ColumnView.preferredWidth: Kirigami.Units.gridUnit * 25
    Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 50
    

    header: Controls.TabBar {
        id: tabBarHeader

        Controls.TabButton {
            text: "First"
        }
        Controls.TabButton {
            text: "Second"
        }
        Controls.TabButton {
            text: "Third"
        }
    }

    RowLayout {
        Controls.TextArea {
            readOnly: true
            text: tabBarHeader.currentIndex
        }

        Controls.TextArea {
            readOnly: true
            text: tabBarFooter.currentIndex
        }
    }

    footer: Controls.TabBar {
        id: tabBarFooter

        Controls.TabButton {
            text: "Fourth"
        }
        Controls.TabButton {
            text: "Fifth"
        }
        Controls.TabButton {
            text: "Sixth"
        }
    }
}
