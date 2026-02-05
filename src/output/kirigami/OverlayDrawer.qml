/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T
import org.kde.kirigami as Kirigami
import org.kde.kirigami.templates as KT

import org.kde.union.impl as Union

KT.OverlayDrawer {
    id: control

    Union.Element.type: "OverlayDrawer"
    Union.Element.hints: Union.ElementHint { name: "modal"; when: control.modal }
    Union.Element.attributes: Union.ElementAttribute {
        name: "edge"
        value: switch (control.edge) {
            case Qt.LeftEdge: return "left"
            case Qt.RightEdge: return "right"
            case Qt.TopEdge: return "top"
            case Qt.BottomEdge: return "bottom"
        }
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

        Kirigami.Separator {
            id: separator

            LayoutMirroring.enabled: false

            anchors {
                top: control.edge === Qt.TopEdge ? parent.bottom : (control.edge === Qt.BottomEdge ? undefined : parent.top)
                left: control.edge === Qt.LeftEdge ? parent.right : (control.edge === Qt.RightEdge ? undefined : parent.left)
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
    }
}
