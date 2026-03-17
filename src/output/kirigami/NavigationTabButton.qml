/* SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 * SPDX-FileCopyrightText: 2021 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T
import org.kde.kirigami.templates as KT

import org.kde.union.impl as Union
import "private" as P

KT.NavigationTabButton {
	id: control

	Union.Element.type: "NavigationTabButton"
	Union.Element.states {
		hovered: control.hovered
		activeFocus: control.activeFocus
		visualFocus: control.visualFocus
		pressed: control.pressed
		checked: control.checked
		enabled: control.enabled
	}

	Union.Element.attributes: [
		P.DisplayAttribute { control: control },
		Union.ElementAttribute {
			name: "direction"
			value: control.T.TabBar.position === T.TabBar.Footer ? "bottom" : "top"
		}
	]

	Union.Positioner.positionItems: [contentItem]

	implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
							Union.Positioner.implicitWidth)
	implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
							 Union.Positioner.implicitHeight)

	hoverEnabled: Application.styleHints.useHoverEffects

	leftPadding: Union.Positioner.padding.left
	rightPadding: Union.Positioner.padding.right
	topPadding: Union.Positioner.padding.top
	bottomPadding: Union.Positioner.padding.bottom

	leftInset: Union.Style.properties.layout.inset.left
	rightInset: Union.Style.properties.layout.inset.right
	topInset: Union.Style.properties.layout.inset.top
	bottomInset: Union.Style.properties.layout.inset.bottom

	font: Union.Style.properties.text.font

	spacing: Union.Style.properties.layout.spacing

	icon {
		color: Union.Style.properties.icon.color
		width: Union.Style.properties.icon.width
		height: Union.Style.properties.icon.height
		name: Union.Style.properties.icon.name
		source: Union.Style.properties.icon.source
	}

	display: T.AbstractButton.TextUnderIcon

	contentItem: Item {
		property int wrapMode: Text.NoWrap

		Union.PositionedItem.positionChildren: true

		Union.Icon {
			Union.PositionedItem.source: Union.PositionerSource.Icon
			control: control
			visible: (name || source.toString()) && control.display != T.AbstractButton.TextOnly
		}

		Text {
			Union.PositionedItem.source: Union.PositionerSource.Text

			text: control.text
			font: control.font
			color: Union.Style.properties.text.color ?? "black"
			elide: wrapMode == Text.NoWrap ? Text.ElideRight : Text.ElideNone
			wrapMode: wrapMode

			visible: control.display != T.AbstractButton.IconOnly && text.length > 0
		}
	}


	background: Union.StyledRectangle {}
}
