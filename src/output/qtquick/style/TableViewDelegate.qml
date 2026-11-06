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
        Union.ElementHint { name: "alternating-colors"; when: (control.tableView?.alternatingRows && control.row % 2 !== 0) ?? false },
        Union.ElementHint { name: "editing"; when: control.editing },
        Union.ElementHint { name: "current"; when: control.current },
        Union.ElementHint { name: "selected"; when: control.selected },
    ]
    
    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom
    
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
        Union.Element.subElement: "Text"

        text: control.model.display ?? ""

        color: Union.Style.properties.display.color ?? control.palette.text
        horizontalAlignment: Union.Style.properties.text.horizontalAlignment ?? Text.AlignLeft
        verticalAlignment: Union.Style.properties.text.verticalAlignment ?? Text.AlignVCenter

        elide: wrapMode == Text.NoWrap && Union.Style.properties.text.elide ? Union.Style.properties.text.elide : Text.ElideNone
        wrapMode: Union.Style.properties.text.wrapMode ?? Text.NoWrap

        visible: !control.editing
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
            anchors.fill: parent
            text: control.model.edit ?? control.model.display ?? ""
            focus: true
            background: Item {}
        }
    }
}
