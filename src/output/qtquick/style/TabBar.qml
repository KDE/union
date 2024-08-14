// SPDX-FileCopyrightText: 2024 Noah Davis <noahadvs@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-or-later

import QtQuick
import QtQuick.Controls.impl as QCCImpl
import QtQuick.Templates as T
import org.kde.union.impl as Union

T.TabBar {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    Union.Element.type: "TabBar"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        enabled: control.enabled
    }
    Union.Element.attributes: position === T.TabBar.Footer ?
        {"tab-position": "south"} : {"tab-position": "north"}

    leftPadding: Union.Style.padding.left
    rightPadding: Union.Style.padding.right
    topPadding: Union.Style.padding.top
    bottomPadding: Union.Style.padding.bottom

    leftInset: Union.Style.margins.left
    rightInset: Union.Style.margins.right
    topInset: Union.Style.margins.top
    bottomInset: Union.Style.margins.bottom

    font: Union.Style.text.font

    spacing: 0 // TODO: make styleable

    contentItem: ListView {
        implicitWidth: contentWidth
        implicitHeight: contentHeight
        model: control.contentModel
        currentIndex: control.currentIndex
        spacing: control.spacing
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.AutoFlickIfNeeded
        snapMode: ListView.SnapToItem
        highlightMoveDuration: 0 // TODO: make styleable
        highlightRangeMode: ListView.ApplyRange
        preferredHighlightBegin: 40
        preferredHighlightEnd: width - 40
        highlightResizeDuration: 0 // TODO: make styleable
        // Some styles use list view highlights, some don't
        highlight: Union.StyledRectangle {
            id: highlight
            Union.Element.type: "TabBar"
            Union.Element.states {
                highlighted: control.currentItem !== null
                enabled: highlight.enabled
            }
            Union.Element.hints: ["itemview-highlight"]
            Union.Element.attributes: control.Union.Element.attributes
        }
    }

    // Some styles use backgrounds, some don't
    background: Union.StyledBackground {}
}
