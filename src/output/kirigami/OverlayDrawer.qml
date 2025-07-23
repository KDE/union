/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T
// import "private" as KP
import org.kde.kirigami as Kirigami
import org.kde.kirigami.templates as KT

import org.kde.union.impl as Union

KT.OverlayDrawer {
    id: control

    Union.Element.type: "OverlayDrawer"
    Union.Element.hints: modal ? ["modal"] : []
    Union.Element.attributes: {
        let result = {}
        switch (edge) {
        case Qt.LeftEdge:
            result.edge = "left"
            break
        case Qt.RightEdge:
            result.edge = "right"
            break
        case Qt.TopEdge:
            result.edge = "top"
            break
        case Qt.BottomEdge:
            result.edge = "bottom"
            break
        }
        return result
    }

//BEGIN Properties
    focus: false
    modal: true
    drawerOpen: !modal
    closePolicy: modal ? T.Popup.CloseOnEscape | T.Popup.CloseOnReleaseOutside : T.Popup.NoAutoClose
    handleVisible: interactive && (modal || !drawerOpen) && (typeof(applicationWindow)===typeof(Function) && applicationWindow() ? applicationWindow().controlsVisible : true)




    // FIXME: set to false when it does not lead to blocking closePolicy.
    // See Kirigami bug: 454119
    interactive: true

    onPositionChanged: {
        if (!modal && !control.peeking && !control.animating) {
            position = 1;
        }
    }

    background: Union.StyledRectangle {

        Item {
            parent: control.handle
            anchors.fill: parent

            Kirigami.ShadowedRectangle {
                id: handleGraphics
                anchors.centerIn: parent

                Kirigami.Theme.colorSet: parent.parent.handleAnchor && parent.parent.handleAnchor.visible ? parent.parent.handleAnchor.Kirigami.Theme.colorSet : Kirigami.Theme.Button

                Kirigami.Theme.backgroundColor: parent.parent.handleAnchor && parent.parent.handleAnchor.visible ? parent.parent.handleAnchor.Kirigami.Theme.backgroundColor : undefined

                Kirigami.Theme.textColor: parent.parent.handleAnchor && parent.parent.handleAnchor.visible ? parent.parent.handleAnchor.Kirigami.Theme.textColor : undefined

                Kirigami.Theme.inherit: false
                color: control.handle.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.backgroundColor

                visible: control.modal && (!parent.parent.handleAnchor || !parent.parent.handleAnchor.visible || control.handle.pressed || control.position > 0)
                opacity: (parent.parent.handleAnchor && parent.parent.handleAnchor.visible) ? control.position : 1


                shadow.color: Qt.rgba(0, 0, 0, control.handle.pressed ? 0.6 : 0.4)
                shadow.yOffset: 1
                shadow.size: Kirigami.Units.gridUnit / 2

                width: Kirigami.Units.iconSizes.smallMedium + Kirigami.Units.smallSpacing * 2
                height: width
                radius: Kirigami.Units.cornerRadius
                Behavior on color {
                    ColorAnimation {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                }
            }
            Loader {
                anchors.centerIn: handleGraphics
                width: height
                height: Kirigami.Units.iconSizes.smallMedium

                Kirigami.Theme.colorSet: handleGraphics.Kirigami.Theme.colorSet
                Kirigami.Theme.backgroundColor: handleGraphics.Kirigami.Theme.backgroundColor
                Kirigami.Theme.textColor: handleGraphics.Kirigami.Theme.textColor

                asynchronous: true

                source: {
                    let edge = control.edge;
                    if (Qt.application.layoutDirection === Qt.RightToLeft) {
                        if (edge === Qt.LeftEdge) {
                            edge = Qt.RightEdge;
                        } else {
                            edge = Qt.LeftEdge;
                        }
                    }

                    // TODO: those 3 still resolve because templates files are installed on
                    // the same place, but we should probably eventually just use normal icons
                    // and drop that custom morphing animation
                    if ((control.handleClosedIcon.source || control.handleClosedIcon.name)
                        && (control.handleOpenIcon.source || control.handleOpenIcon.name)) {
                        return Qt.resolvedUrl("templates/private/GenericDrawerIcon.qml");
                    } else if (edge === Qt.LeftEdge) {
                        return Qt.resolvedUrl("templates/private/MenuIcon.qml");
                    } else if (edge === Qt.RightEdge && control instanceof Kirigami.ContextDrawer) {
                        return Qt.resolvedUrl("templates/private/ContextIcon.qml");
                    } else {
                        return "";
                    }
                }
                onItemChanged: {
                    if (item) {
                        item.drawer = control;
                        item.color = Qt.binding(() => control.handle.pressed
                            ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor);
                    }
                }
            }
        }

        Kirigami.Separator {
            id: separator

            LayoutMirroring.enabled: false

            anchors {
                top: control.edge === Qt.TopEdge ? parent.bottom : (root.edge === Qt.BottomEdge ? undefined : parent.top)
                left: control.edge === Qt.LeftEdge ? parent.right : (root.edge === Qt.RightEdge ? undefined : parent.left)
                right: control.edge === Qt.RightEdge ? parent.left : (control.edge === Qt.LeftEdge ? undefined : parent.right)
                bottom: control.edge === Qt.BottomEdge ? parent.top : (control.edge === Qt.TopEdge ? undefined : parent.bottom)
                topMargin: segmentedSeparator.height
            }

            visible: !control.modal

            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.Header
        }

        Item {
            id: segmentedSeparator

            // an alternative to segmented style is full height
            readonly property bool shouldUseSegmentedStyle: {
                if (control.edge !== Qt.LeftEdge && control.edge !== Qt.RightEdge) {
                    return false;
                }
                if (control.collapsed) {
                    return false;
                }
                // compatible header
                const header = control.header ?? null;
                if (header instanceof T.ToolBar || header instanceof KT.AbstractApplicationHeader) {
                    return true;
                }
                // or compatible content
                if (control.contentItem instanceof ColumnLayout && control.contentItem.children[0] instanceof T.ToolBar) {
                    return true;
                }
                return false;
            }

            anchors {
                top: parent.top
                left: separator.left
                right: separator.right
            }

            height: {
                if (control.edge !== Qt.LeftEdge && control.edge !== Qt.RightEdge) {
                    return 0;
                }
                if (typeof applicationWindow === "undefined") {
                    return 0;
                }
                const window = applicationWindow();
                const globalToolBar = window.pageStack?.globalToolBar;
                if (!globalToolBar) {
                    return 0;
                }

                return globalToolBar.preferredHeight;
            }

            visible: separator.visible

            Kirigami.Separator {
                LayoutMirroring.enabled: false

                anchors {
                    fill: parent
                    topMargin: segmentedSeparator.shouldUseSegmentedStyle ? Kirigami.Units.largeSpacing : 0
                    bottomMargin: segmentedSeparator.shouldUseSegmentedStyle ? Kirigami.Units.largeSpacing : 0
                }

                Behavior on anchors.topMargin {
                    NumberAnimation {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                }

                Behavior on anchors.bottomMargin {
                    NumberAnimation {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                }

                Kirigami.Theme.inherit: false
                Kirigami.Theme.colorSet: Kirigami.Theme.Header
            }
        }
/*
        KP.EdgeShadow {
            z: -2
            visible: control.modal
            edge: control.edge
            anchors {
                right: control.edge === Qt.RightEdge ? parent.left : (control.edge === Qt.LeftEdge ? undefined : parent.right)
                left: control.edge === Qt.LeftEdge ? parent.right : (control.edge === Qt.RightEdge ? undefined : parent.left)
                top: control.edge === Qt.TopEdge ? parent.bottom : (control.edge === Qt.BottomEdge ? undefined : parent.top)
                bottom: control.edge === Qt.BottomEdge ? parent.top : (control.edge === Qt.TopEdge ? undefined : parent.bottom)
            }

            opacity: control.position === 0 ? 0 : 1

            Behavior on opacity {
                NumberAnimation {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InOutQuad
                }
            }
        }*/
    }
}
