// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.TreeViewDelegate {
    id: control
    Union.Element.type: "TreeViewDelegate"
    Union.Element.hints: {
        let hints = [];
        if (control.treeView.alternatingRows && control.row % 2) {
            hints.push("alternatingRows");
        }
        if (control.expanded) {
            hints.push("expanded");
        }
        if (control.editing) {
            hints.push("editing");
        }
        return hints;
    }
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.down
        enabled: control.enabled
        highlighted: control.highlighted
    }

    required property var model
    required property int row
    readonly property real __contentIndent: !isTreeNode ? 0 : (depth * indentation) + (indicator ? indicator.width + spacing : 0)

    implicitWidth: leftPadding + __contentIndent + implicitContentWidth + rightPadding
    implicitHeight: Math.max(indicator ? indicator.height : 0, implicitContentHeight) + topPadding + bottomPadding

    topPadding: Union.Style.properties.layout.padding.top
    rightPadding: Union.Style.properties.layout.padding.right
    leftPadding: !mirrored ? Union.Style.properties.layout.padding.left + __contentIndent : width - Union.Style.properties.layout.padding.left - __contentIndent - implicitContentWidth
    bottomPadding: Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    highlighted: control.selected || control.current || ((control.treeView.selectionBehavior === TableView.SelectRows || control.treeView.selectionBehavior === TableView.SelectionDisabled) && control.row === control.treeView.currentRow)

    indicator: Union.Icon {
        Union.Element.type: "Indicator"
        readonly property real __indicatorIndent: control.leftMargin + (control.depth * control.indentation)
        x: !control.mirrored ? __indicatorIndent : control.width - __indicatorIndent - width
        y: (control.height - height) / 2
        color: Union.Style.properties.icon.color
        width: Union.Style.properties.icon.width
        height: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
    }

    background: Union.StyledRectangle {}

    contentItem: Text {
        clip: false
        text: control.model.display ?? ""
        elide: Text.ElideRight
        visible: !control.editing
        color: Union.Style.properties.text.color
        horizontalAlignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal)
        verticalAlignment: Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)
    }

    TableView.editDelegate: FocusScope {
        width: parent.width
        height: parent.height

        readonly property int __role: {
            let model = control.treeView.model;
            let index = control.treeView.index(row, column);
            let editText = model.data(index, Qt.EditRole);
            return editText !== undefined ? Qt.EditRole : Qt.DisplayRole;
        }

        TextField {
            id: textField
            horizontalAlignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal)
            verticalAlignment: Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)
            // Remove the TextField background, we want to use the Control background.
            background: Item {}
            x: control.contentItem.x
            y: (parent.height - height) / 2
            width: control.contentItem.width
            text: control.treeView.model.data(control.treeView.index(row, column), __role)
            focus: true
        }

        TableView.onCommit: {
            let index = TableView.view.index(row, column);
            TableView.view.model.setData(index, textField.text, __role);
        }

        Component.onCompleted: textField.selectAll()
    }
}
