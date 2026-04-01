// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Layouts

import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

Kirigami.Page {
	title: "Calendar QtQuick.Controls"

	Kirigami.ColumnView.interactiveResizeEnabled: true
	Kirigami.ColumnView.minimumWidth: Kirigami.Units.gridUnit * 10
	Kirigami.ColumnView.preferredWidth: Kirigami.Units.gridUnit * 25
	Kirigami.ColumnView.maximumWidth: Kirigami.Units.gridUnit * 50

	ColumnLayout {
		anchors.fill: parent

		GridLayout {
			Layout.alignment: Qt.AlignCenter
			Layout.maximumWidth: 800
			Layout.maximumHeight: 800
			columns: 2

			Controls.DayOfWeekRow {
				id: dayOfWeekRow
				locale: grid.locale

				Layout.column: 1
				Layout.fillWidth: true
			}

			Controls.WeekNumberColumn {
				month: grid.month
				year: grid.year
				locale: grid.locale

				Layout.fillHeight: true
			}

			Controls.MonthGrid {
				id: grid
				month: window.currentDate.getMonth()
				year: window.currentDate.getFullYear()

				Layout.fillWidth: true
				Layout.fillHeight: true
			}
		}
		Item {
			Layout.fillHeight: true
		}
	}
}
