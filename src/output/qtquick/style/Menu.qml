// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.Menu {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

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

    delegate: MenuItem { }

    contentItem: ListView {
        id: view

        readonly property bool hasAnySubmenu: contentItem.visibleChildren.some(item => item?.arrow?.visible ?? false)

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

    background: Union.StyledRectangle {} 
}
