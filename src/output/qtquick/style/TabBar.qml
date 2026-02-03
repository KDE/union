// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Noah Davis <noahadvs@gmail.com>

pragma ComponentBehavior: Bound;

import QtQuick
import QtQuick.Templates as T
import org.kde.union.impl as Union

T.TabBar {
    id: control

    Union.Element.type: "TabBar"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        enabled: control.enabled
    }
    Union.Element.attributes: Union.ElementAttribute {
        name: "tab-position"
        value: control.position === T.TabBar.Footer ? "bottom" : "top"
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    spacing: Union.Style.properties.layout.spacing

    font: Union.Style.properties.text.font

    contentItem: ListView {
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
            Union.Element.type: "TabBarHighlight"
            Union.Element.states {
                highlighted: control.currentItem !== null
                enabled: highlight.enabled
            }
            Union.Element.hints: Union.ElementHint { name: "itemview-highlight" }
            Union.Element.attributes: Union.ElementAttribute {
                name: "tab-position"
                value: control.position === T.TabBar.Footer ? "bottom" : "top"
            }
        }
    }

    // Some styles use backgrounds, some don't
    background: Union.StyledRectangle {}
}
