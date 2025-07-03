// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.MenuBar {
	id: control

	Union.Element.type: "MenuBar"

	implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
							implicitContentWidth + leftPadding + rightPadding)
	implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
							 implicitContentHeight + topPadding + bottomPadding)

	topPadding: SafeArea.margins.top
	leftPadding: SafeArea.margins.left
	rightPadding: SafeArea.margins.right
	bottomPadding: SafeArea.margins.bottom

	delegate: MenuBarItem { }

	contentItem: Row {
		spacing: control.spacing
		Repeater {
			model: control.contentModel
		}
	}

	background: Rectangle {
		implicitHeight: 40
		color: control.palette.button
	}
}
