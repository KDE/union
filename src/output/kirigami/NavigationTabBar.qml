/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 * SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import org.kde.kirigami.templates as KT
import org.kde.union.impl as Union

KT.NavigationTabBar {
	id: control

	Union.Element.type: "NavigationTabBar"
	Union.Element.states {
		hovered: control.hovered
		activeFocus: control.activeFocus
		visualFocus: control.visualFocus
		enabled: control.enabled
	}
	Union.Element.hints: [
		Union.ElementHint { name: "header"; when: control.position === KT.NavigationTabBar.Header },
		Union.ElementHint { name: "footer"; when: control.position === KT.NavigationTabBar.Footer },
	]

	implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, contentWidth)
	implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, contentHeight + topPadding + bottomPadding)

	leftPadding: Math.floor(Math.max(0, width - control.maximumContentWidth) / 2) + Union.Style.properties.layout.padding.left
	rightPadding: Math.floor(Math.max(0, width - control.maximumContentWidth) / 2) + Union.Style.properties.layout.padding.right
	topPadding: Union.Style.properties.layout.padding.top
	bottomPadding:  Union.Style.properties.layout.padding.bottom

	leftInset: Union.Style.properties.layout.inset.left
	rightInset: Union.Style.properties.layout.inset.right
	topInset: Union.Style.properties.layout.inset.top
	bottomInset: Union.Style.properties.layout.inset.bottom
	background: Union.StyledRectangle { }
}
