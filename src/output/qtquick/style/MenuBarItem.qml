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
	font: Union.Style.properties.text.font
	palette: Union.Style.properties.palette.quickPalette

	implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
							implicitContentWidth + leftPadding + rightPadding)
	implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
							 implicitContentHeight + topPadding + bottomPadding,
							 implicitIndicatorHeight + topPadding + bottomPadding)

	spacing: Union.Style.properties.layout.spacing
	topPadding: Union.Style.properties.layout.padding.top
	bottomPadding: Union.Style.properties.layout.padding.bottom
	leftPadding: Union.Style.properties.layout.padding.left
	rightPadding: Union.Style.properties.layout.padding.right

	icon.width: Union.Style.properties.icon.width
	icon.height: Union.Style.properties.icon.height
	icon.color: control.palette.buttonText

	contentItem: IconLabel {
		font: control.font
		spacing: control.spacing
		mirrored: control.mirrored
		display: control.display
		alignment: Qt.AlignLeft

		icon: control.icon
		text: control.text
		color: control.palette.buttonText
	}

	background: Union.StyledRectangle { }
}
