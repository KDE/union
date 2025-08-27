// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.Slider {
	id: control

	Union.Element.type: "Slider"
	Union.Element.attributes: {
		"orientation": control.horizontal ? "horizontal" : "vertical"
	}
	implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
							implicitHandleWidth + leftPadding + rightPadding)
	implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
							 implicitHandleHeight + topPadding + bottomPadding)

	leftPadding: Union.Style.properties.layout.padding.left
	rightPadding: Union.Style.properties.layout.padding.right
	topPadding: Union.Style.properties.layout.padding.top
	bottomPadding: Union.Style.properties.layout.padding.bottom

	leftInset: Union.Style.properties.layout.inset.left
	rightInset: Union.Style.properties.layout.inset.right
	topInset: Union.Style.properties.layout.inset.top
	bottomInset: Union.Style.properties.layout.inset.bottom

	palette: Union.Style.properties.palette.quickPalette

	handle: Rectangle {
		Union.Element.type: "Handle"
		x: control.leftPadding + (control.horizontal ? control.visualPosition * (control.availableWidth - width) : (control.availableWidth - width) / 2)
		y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : control.visualPosition * (control.availableHeight - height))
		implicitWidth: 28
		implicitHeight: 28
		radius: width / 2
		color: control.pressed ? control.palette.light : control.palette.window
		border.width: control.visualFocus ? 2 : 1
		border.color: {
			if (activeFocus) {
				return control.palette.highlight
			}
				return control.enabled ? control.palette.mid : control.palette.midlight
		}
	}

	background: Union.StyledRectangle {
		Union.Element.type: "SliderBackground"
		x: control.leftPadding + (control.horizontal ? 0 : (control.availableWidth - width) / 2)
		y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : 0)

		Union.StyledRectangle {
			Union.Element.type: "SliderBackgroundFill"
			y: control.horizontal ? 0 : control.visualPosition * parent.height
			width: control.horizontal ? control.position * parent.width : parent.height
			height: control.horizontal ? parent.width : control.position * parent.height
		}
	}

	Component.onCompleted: {
		console.warn("bg", background.width, background.implicitWidth, background.height, background.implicitHeight, control.availableHeight, control.availableWidth);
	}

}
