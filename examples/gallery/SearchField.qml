// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick

Item {
    id: root

    property string module

    implicitWidth: searchField.implicitWidth
    implicitHeight: searchField.implicitHeight

    ListModel {
        id : fruitModel
        ListElement { name: "Apple"; color: "green" }
        ListElement { name: "Cherry"; color: "red" }
        ListElement { name: "Banana"; color: "yellow" }
        ListElement { name: "Orange"; color: "orange" }
        ListElement { name: "WaterMelon"; color: "pink" }
    }

    SortFilterProxyModel {
        id: fruitFilter
        model: fruitModel
        sorters: [
            RoleSorter {
                roleName: "name"
            }
        ]
        filters: [
            RegExpFilter {
                roleName: "name"
                regExp: {
                    if (searchField.item && searchField.item.text) {
                        return new RegExp(searchField.item.text, "i")
                    } else {
                        return new RegExp()
                    }
                }
            }
        ]
    }

    TryControl {
        id: searchField

        anchors.fill: parent

        name: "SearchField"
        module: root.module
        properties: {
            "suggestionModel": fruitModel,
            "textRole": "name",
            "placeholderText": "Search…"
        }
    }
}
