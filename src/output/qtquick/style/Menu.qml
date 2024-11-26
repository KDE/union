// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
// import QtQuick.Controls.impl
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.Menu {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding + (-leftInset) + (-rightInset))
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding + (-topInset) + (-bottomInset))

    Union.Element.type: "Menu"
    Union.Element.states {
        activeFocus: control.activeFocus
        enabled: control.enabled
    }

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    leftMargin: Union.Style.properties.layout.margins.left
    rightMargin: Union.Style.properties.layout.margins.right
    topMargin: Union.Style.properties.layout.margins.top
    bottomMargin: Union.Style.properties.layout.margins.bottom

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

    background: Union.StyledRectangle { }
}
