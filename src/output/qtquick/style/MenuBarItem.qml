// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.MenuBarItem {
	id: control

	Union.Element.type: "MenuBarItem"

	palette: Union.Style.properties.palette.quickPalette

	implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
							implicitContentWidth + leftPadding + rightPadding)
	implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
							 implicitContentHeight + topPadding + bottomPadding,
							 implicitIndicatorHeight + topPadding + bottomPadding)

	spacing: 6
	padding: 6
	leftPadding: 12
	rightPadding: 16

	icon.width: 24
	icon.height: 24
	icon.color: control.palette.buttonText

	contentItem: IconLabel {
		font: Union.Style.properties.text.font
		palette: Union.Style.properties.palette.quickPalette
		spacing: control.spacing
		mirrored: control.mirrored
		display: control.display
		alignment: Qt.AlignLeft

		icon: control.icon
		text: control.text
		color: control.palette.buttonText
	}

	background: Rectangle {
		implicitWidth: 40
		implicitHeight: 40
		palette: Union.Style.properties.palette.quickPalette
		color: control.down || control.highlighted ? control.palette.mid : "transparent"
		border.color: control.palette.dark
		border.width: Qt.styleHints.accessibility.contrastPreference === Qt.HighContrast &&
		control.highlighted ? 1 : 0
	}
}
