// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.Dial {
	id: control

	Union.Element.type: "Dial"
	Union.Element.states {
		hovered: control.hovered
		activeFocus: control.activeFocus
		visualFocus: control.visualFocus
		enabled: control.enabled
		pressed: control.pressed
	}
	implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
							implicitContentWidth + leftPadding + rightPadding)
	implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
							 implicitContentHeight + topPadding + bottomPadding)

	leftPadding: Union.Style.properties.layout.padding.left
	rightPadding: Union.Style.properties.layout.padding.right
	topPadding: Union.Style.properties.layout.padding.top
	bottomPadding:  Union.Style.properties.layout.padding.bottom

	leftInset: Union.Style.properties.layout.inset.left
	rightInset: Union.Style.properties.layout.inset.right
	topInset: Union.Style.properties.layout.inset.top
	bottomInset: Union.Style.properties.layout.inset.bottom

	font: Union.Style.properties.text.font

	background: Union.StyledRectangle {
		implicitWidth: control.Union.Style.properties.layout.width
		implicitHeight: control.Union.Style.properties.layout.height
		x: control.width / 2 - width / 2
		y: control.height / 2 - height / 2
		width: Math.max(64, Math.min(control.width, control.height))
		height: width
		opacity: control.enabled ? 1 : 0.3
	}

	handle: Union.StyledRectangle {
		Union.Element.type: "Handle"
		Union.Element.states {
			hovered: control.hovered
			activeFocus: control.handle.activeFocus
			visualFocus: control.visualFocus
			pressed: control.pressed
			enabled: control.handle.enabled
		}
		x: control.background.x + control.background.width / 2 - width / 2
		y: control.background.y + control.background.height / 2 - height / 2
		transform: [
			Translate {
				y: -Math.min(control.background.width, control.background.height) * 0.45
				+ (control.handle ? control.handle.height / 2 : 0)
			},
			Rotation {
				angle: control.angle
				origin.x: control.handle ? control.handle.width / 2 : 0
				origin.y: control.handle ? control.handle.height / 2 : 0
			}
		]
	}
}
