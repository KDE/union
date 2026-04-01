// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2022 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.AbstractWeekNumberColumn {
	id: control

	Union.Element.type: "WeekNumberColumn"
	Union.Element.states {
		hovered: control.hovered
		activeFocus: control.activeFocus
		visualFocus: control.visualFocus
		enabled: control.enabled
	}
	implicitWidth: Math.max(background ? background.implicitWidth : 0,
							contentItem.implicitWidth + leftPadding + rightPadding)
	implicitHeight: Math.max(background ? background.implicitHeight : 0,
							 contentItem.implicitHeight + topPadding + bottomPadding)

	leftPadding: Union.Style.properties.layout.padding.left
	rightPadding: Union.Style.properties.layout.padding.right
	topPadding: Union.Style.properties.layout.padding.top
	bottomPadding:  Union.Style.properties.layout.padding.bottom

	leftInset: Union.Style.properties.layout.inset.left
	rightInset: Union.Style.properties.layout.inset.right
	topInset: Union.Style.properties.layout.inset.top
	bottomInset: Union.Style.properties.layout.inset.bottom

	font: Union.Style.properties.text.font

	spacing: Union.Style.properties.layout.spacing

	delegate: Label {
		text: weekNumber
		font: control.font
		color: Union.Style.properties.text.color ?? "black"
		horizontalAlignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal)
		verticalAlignment: Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)

		required property int weekNumber
	}

	contentItem: Column {
		spacing: control.spacing
		Repeater {
			model: control.source
			delegate: control.delegate
		}
	}

	background: Union.StyledRectangle {}
}
