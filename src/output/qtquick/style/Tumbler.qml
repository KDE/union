// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.Tumbler {
	id: control
	Union.Element.type: "Tumbler"
	Union.Element.states {
		hovered: control.hovered
		activeFocus: control.activeFocus
		visualFocus: control.visualFocus
		enabled: control.enabled
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

	spacing: Union.Style.properties.layout.spacing


	delegate: Text {
		Union.Element.type: "Text"
		Union.Element.states {
			highlighted: PathView.isCurrentItem
		}
		font: control.font
		color: Union.Style.properties.text.color ?? "black"
		horizontalAlignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal)
		verticalAlignment: Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)
		// We use required property here to satisfy qmllint, but that means
		// we also need to declare the index for the attached properties
		// (see QQuickTumblerAttachedPrivate::init).
		required property var modelData
		required property int index
		text: modelData
	}

	contentItem: Item {
		id: pathContainer
		implicitWidth: control.Union.Style.properties.layout.width
		implicitHeight: control.Union.Style.properties.layout.height
		readonly property real delegateHeight: control.availableHeight / control.visibleItemCount

		clip: true
		PathView {
			anchors.fill: parent
			model: control.model
			delegate: control.delegate
			snapMode: PathView.SnapToItem
			pathItemCount: 5
			preferredHighlightBegin: 0.5
			preferredHighlightEnd: 0.5
			path: Path {
				startX: control.contentItem.width / 2
				startY: -pathContainer.delegateHeight / 2

				PathLine {
					x: control.contentItem.width / 2
					y: (control.visibleItemCount + 1) * pathContainer.delegateHeight - pathContainer.delegateHeight / 2
				}
			}
		}
	}


	background: Union.StyledRectangle { }
}
