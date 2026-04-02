// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

Item {
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
            FunctionFilter {
                component CustomData: QtObject { property string name }
                property var regExp: new RegExp(fruitSearch.text, "i")
                onRegExpChanged: invalidate()
                function filter(data: CustomData): bool {
                    return regExp.test(data.name);
                }
            }
        ]
    }

    Controls.SearchField {
        id: fruitSearch
        implicitWidth: 200
        suggestionModel: fruitFilter
        textRole: "name"
    }
}