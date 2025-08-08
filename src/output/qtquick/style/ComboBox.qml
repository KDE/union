// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.ComboBox {
	id: control

	Union.Element.type: "ComboBox"
	Union.Element.states {
		hovered: control.hovered
		activeFocus: control.activeFocus
		visualFocus: control.visualFocus
		pressed: control.down
		enabled: control.enabled
		highlighted: control.highlighted
	}

	implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
							implicitContentWidth + leftPadding + rightPadding)
	implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
							 implicitContentHeight + topPadding + bottomPadding,
						  implicitIndicatorHeight + topPadding + bottomPadding)

	leftPadding: Union.Positioner.padding.left + (!control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)
	rightPadding: Union.Positioner.padding.right  + (control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)
	topPadding: Union.Positioner.padding.top
	bottomPadding:  Union.Positioner.padding.bottom

	leftInset: Union.Style.properties.layout.inset.left
	rightInset: Union.Style.properties.layout.inset.right
	topInset: Union.Style.properties.layout.inset.top
	bottomInset: Union.Style.properties.layout.inset.bottom

	font: Union.Style.properties.text.font

	spacing: Union.Style.properties.layout.spacing

	palette: Union.Style.properties.palette.quickPalette

	delegate: ItemDelegate {
		required property var model
		required property int index

		width: ListView.view.width
		text: model[control.textRole]
		palette.text: control.palette.text
		palette.highlightedText: control.palette.highlightedText
		font.weight: control.currentIndex === index ? Font.DemiBold : Font.Normal
		highlighted: control.highlightedIndex === index
		hoverEnabled: control.hoverEnabled
	}

	indicator: IconImage {
		anchors.centerIn: parent
		width: Union.Style.properties.icon.width
		height: Union.Style.properties.icon.height
		name: Union.Style.properties.icon.name ?? ""
		palette: Union.Style.properties.palette.quickPalette
		color: palette.buttonText
	}

	contentItem: T.TextField {
		leftPadding: !control.mirrored ? 12 : control.editable && activeFocus ? 3 : 1
		rightPadding: control.mirrored ? 12 : control.editable && activeFocus ? 3 : 1
		topPadding: 6 - control.padding
		bottomPadding: 6 - control.padding

		text: control.editable ? control.editText : control.displayText

		enabled: control.editable
		autoScroll: control.editable
		readOnly: control.down
		inputMethodHints: control.inputMethodHints
		validator: control.validator
		selectByMouse: control.selectTextByMouse

		color: control.editable ? control.palette.text : control.palette.buttonText
		selectionColor: control.palette.highlight
		selectedTextColor: control.palette.highlightedText
		verticalAlignment: Text.AlignVCenter

		background: Rectangle {
			visible: control.enabled && control.editable && !control.flat
			border.width: parent && parent.activeFocus ? 2 : 1
			color: control.palette.base
		}
	}

	background: Union.StyledRectangle {
		implicitWidth: 140
		implicitHeight: 40

		visible: !control.flat || control.down
	}

	popup: T.Popup {
		y: control.height
		width: control.width
		height: Math.min(contentItem.implicitHeight, control.Window.height - topMargin - bottomMargin)
		topMargin: 6
		bottomMargin: 6
		palette: control.palette

		contentItem: ListView {
			clip: true
			implicitHeight: contentHeight
			model: control.delegateModel
			currentIndex: control.highlightedIndex
			highlightMoveDuration: 0

			Rectangle {
				z: 10
				width: parent.width
				height: parent.height
				color: "transparent"
				border.color: control.palette.mid
			}

			// Show a contour around the highlighted item in high contrast mode
			Rectangle {
				property Item highlightedItem: parent ? parent.itemAtIndex(control.highlightedIndex) : null
				visible: highlightedItem
				z: 11
				x: highlightedItem ? highlightedItem.x : 0
				y: highlightedItem ? highlightedItem.y : 0
				width: highlightedItem ? highlightedItem.width : 0
				height: highlightedItem ? highlightedItem.height : 0
				color: "transparent"
				border.color: control.palette.dark
			}

			T.ScrollIndicator.vertical: ScrollIndicator { }
		}

		background: Rectangle {
			color: control.palette.window
		}
	}
}

