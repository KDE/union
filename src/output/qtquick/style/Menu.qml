// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.Menu {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + (Union.Style.properties.layout.inset.left ?? 0) + (Union.Style.properties.layout.inset.right ?? 0),
                            contentWidth + leftPadding + rightPadding) + (-leftInset) + (-rightInset)
    implicitHeight: Math.max(implicitBackgroundHeight + (Union.Style.properties.layout.inset.top ?? 0) + (Union.Style.properties.layout.inset.bottom ?? 0),
                             contentHeight + topPadding + bottomPadding) + (-topInset) + (-bottomInset)

    Union.Element.type: "Menu"
    Union.Element.states {
        activeFocus: control.activeFocus
        enabled: control.enabled
    }

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: -Union.Style.properties.layout.margins.left
    rightInset: -Union.Style.properties.layout.margins.right
    topInset: -Union.Style.properties.layout.margins.top
    bottomInset: -Union.Style.properties.layout.margins.bottom

    popupType: T.Popup.Window

    delegate: MenuItem { }

    contentItem: ListView {
        id: view

        // Helper property for MenuItem to indicate whether it should reserve
        // space for the icon even if it does not have an icon itself. This
        // ensures equal alignment when only some items have icons.
        readonly property bool hasIcons: contentItem.children.some(item => item.icon?.name || item.icon?.source.toString())
        // Helper property for MenuItem to indicate whether it should reserve
        // space for the checkable indicator even if it is not checkable itself.
        // This ensures equal alignment when only some items are checkable.
        readonly property bool hasCheckable: contentItem.children.some(item => item.checkable)

        implicitWidth: contentItem.children.reduce((accumulator, item) => Math.max(accumulator, item.implicitWidth), 0)
        implicitHeight: contentHeight

        model: control.contentModel
        interactive: Window.window
                     ? contentHeight + control.topPadding + control.bottomPadding > control.height
                     : false
        clip: true
        currentIndex: control.currentIndex

        ScrollIndicator.vertical: ScrollIndicator {}
    }

    background: Item {
        // Workaround for Popup margins not working as expected. We need a window
        // that is larger than the actual background, but we still want the
        // background at the right position. We use margins to indicate how much
        // bigger the background needs to be, then correct for that here by
        // insetting the actual StyledRectangle background.

        implicitWidth: Union.Style.properties.layout.width ?? 0
        implicitHeight: Union.Style.properties.layout.height ?? 0

        Union.StyledRectangle {
            anchors {
                fill: parent
                leftMargin: (Union.Style.properties.layout.margins.left ?? 0) + (Union.Style.properties.layout.inset.left ?? 0)
                rightMargin: (Union.Style.properties.layout.margins.right ?? 0) + (Union.Style.properties.layout.inset.right ?? 0)
                topMargin: (Union.Style.properties.layout.margins.top ?? 0) + (Union.Style.properties.layout.inset.top ?? 0)
                bottomMargin: (Union.Style.properties.layout.margins.bottom ?? 0) + (Union.Style.properties.layout.inset.bottom ?? 0)
            }
        }
    }
}
