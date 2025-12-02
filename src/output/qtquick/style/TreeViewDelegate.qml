// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.TreeViewDelegate {
    id: control

    required property var model
    required property int row
    readonly property real __contentIndent: !isTreeNode ? 0 : (depth * indentation) + (indicator ? indicator.width + spacing : 0)

    Union.Element.type: "TreeViewDelegate"
    Union.Element.hints: {
        let result = [];
        if (control.treeView.alternatingRows && control.row % 2 !== 0) {
            result.push("alternatingRows");
        }
        if (control.expanded) {
            result.push("expanded");
        }
        if (control.editing) {
            result.push("editing");
        }
        return result;
    }
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus
        visualFocus: control.visualFocus
        pressed: control.down
        enabled: control.enabled
        highlighted: control.highlighted
    }
    Union.Positioner.positionItems: [indentItem, indicator, contentItem]

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, Union.Positioner.implicitWidth) + __contentIndent
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, Union.Positioner.implicitHeight)

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding: Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    spacing: Union.Style.properties.layout.spacing

    highlighted: control.selected || control.current || ((control.treeView.selectionBehavior === TableView.SelectRows || control.treeView.selectionBehavior === TableView.SelectionDisabled) && control.row === control.treeView.currentRow)

    Item {
        id: indentItem
        visible: control.isTreeNode
        Union.PositionedItem.source: Union.PositionerSource.Icon
        implicitHeight: Union.Style.properties.icon.height
        implicitWidth: (control.depth * control.indentation) + (control.indicator.visible ? 0 : control.indicator.width + control.leftPadding)
    }

    indicator: Union.Icon {
        Union.Element.type: "Indicator"
        Union.PositionedItem.source: Union.PositionerSource.Icon
        color: Union.Style.properties.icon.color
        implicitWidth: Union.Style.properties.icon.width
        implicitHeight: Union.Style.properties.icon.height
        name: Union.Style.properties.icon.name
    }

    background: Union.StyledRectangle {}

    contentItem: Text {
        Union.PositionedItem.source: Union.PositionerSource.Text
        clip: false
        text: control.model.display ?? ""
        elide: Text.ElideRight
        visible: !control.editing
        color: Union.Style.properties.text.color
        horizontalAlignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal)
        verticalAlignment: Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)
    }

    TableView.editDelegate: FocusScope {
        Union.PositionedItem.source: Union.PositionerSource.Text

        width: parent.width
        height: parent.height

        readonly property int __role: {
            let model = control.treeView.model;
            let index = control.treeView.index(control.row, control.column);
            let editText = model.data(index, Qt.EditRole);
            return editText !== undefined ? Qt.EditRole : Qt.DisplayRole;
        }

        TextField {
            id: textField
            horizontalAlignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal)
            verticalAlignment: Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)
            // Remove the TextField background, we want to use the Control background.
            background: Item {}
            width: control.contentItem.width
            text: control.treeView.model.data(control.treeView.index(control.row, control.column), parent.__role)
            focus: true
        }

        TableView.onCommit: {
            let index = TableView.view.index(control.row, control.column);
            TableView.view.model.setData(index, textField.text, __role);
        }

        Component.onCompleted: textField.selectAll()
    }
}
