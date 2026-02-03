// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

import QtQuick
import Qt.labs.qmlmodels as QtLabsQmlModels
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.TableViewDelegate {
    id: control
    required property int column
    required property int row
    required property var model

    Union.Element.type: "TableViewDelegate"
    Union.Element.states {
        hovered: control.hovered
        activeFocus: control.activeFocus || control.current
        visualFocus: control.visualFocus
        pressed: control.down
        enabled: control.enabled
        highlighted: control.highlighted
    }
    Union.Element.hints: [
        Union.ElementHint { name: "alternating-colors"; when: control.tableView?.alternatingRows && control.row % 2 !== 0 },
        Union.ElementHint { name: "editing"; when: control.editing }
    ]
    
    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding: Union.Positioner.padding.bottom
    
    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom
    
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                            implicitContentHeight + topPadding + bottomPadding)
    
    highlighted: control.selected
    
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
        
        TableView.onCommit: {
            let model = control.tableView.model
            if (!model) {
                return
            }
            let succeed = false;
            const index = model.index(control.row, control.column);
            if (model instanceof QtLabsQmlModels.TableModel) {
                succeed = model.setData(index, "edit", textField.text)
            }
            else {
                succeed = model.setData(index, textField.text, Qt.EditRole)
                if (!succeed) {
                    console.warn("The model does not allow setting the EditRole data.")
                }
            }
        }
        
        Component.onCompleted: textField.selectAll()
        
        TextField {
            id: textField
            horizontalAlignment: Union.Alignment.toQtHorizontal(Union.Style.properties.text.alignment.horizontal)
            verticalAlignment: Union.Alignment.toQtVertical(Union.Style.properties.text.alignment.vertical)
            anchors.fill: parent
            text: control.model.edit ?? control.model.display ?? ""
            focus: true
            // Remove the TextField background, we want to use the Control background.
            background: Item {}
        }
    }
}
