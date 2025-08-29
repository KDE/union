// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import org.kde.union.impl as Union

T.Popup {
	id: control
	Union.Element.type: "Popup"
	Union.Element.states {
		activeFocus: control.activeFocus
		enabled: control.enabled
	}

	implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
							implicitContentWidth + leftPadding + rightPadding)
	implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
							 implicitContentHeight + topPadding + bottomPadding)

	leftPadding: Union.Positioner.padding.left
	rightPadding: Union.Positioner.padding.right
	topPadding: Union.Positioner.padding.top
	bottomPadding:  Union.Positioner.padding.bottom


	background: Union.StyledRectangle {}

	T.Overlay.modal: Union.StyledRectangle {
		Union.Element.type: "Overlay"
		Union.Element.hints: ["modal"]
	}

	T.Overlay.modeless: Union.StyledRectangle {
		Union.Element.type: "Overlay"
		Union.Element.hints: ["modeless"]
	}
}