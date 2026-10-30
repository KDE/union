// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami

Item {
    id: root

    property string name
    property string module
    property var properties
    property var item

    property bool error

    function load() {
        if (!name || !module) {
            return
        }

        if (item) {
            item.destroy()
        }

        let component = Qt.createComponent(module, name, Qt.Asynchronous)
        if (component.status == Component.Ready) {
            error = false
            item = component.createObject(root, properties)
        } else {
            error = true
            item = errorPlaceholder.createObject(root, { text: component.errorString() })
        }
    }

    implicitWidth: error ? 300 : item?.implicitWidth ?? 0
    implicitHeight: item?.implicitHeight ?? 0

    onNameChanged: load()
    onModuleChanged: load()
    onPropertiesChanged: load()

    Binding {
        target: root.item
        property: "width"
        value: root.width
    }

    Binding {
        target: root.item
        property: "height"
        value: root.height
    }

    Component {
        id: errorPlaceholder

        Label {
            wrapMode: Text.Wrap
            verticalAlignment: Text.AlignVCenter
            leftPadding: 8
            rightPadding: 8
            topPadding: 8
            bottomPadding: 8

            background: Rectangle {
                radius: Kirigami.Units.cornerRadius
                color: Kirigami.Theme.negativeBackgroundColor
                border.width: 1
                border.color: Kirigami.Theme.negativeTextColor
            }
        }
    }
}
