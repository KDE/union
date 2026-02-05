/*
 *  SPDX-FileCopyrightText: 2012 Sebastian Kügler <sebas@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.Label {
    id: control

    property int level: 1

    enum Type {
        Normal,
        Primary,
        Secondary
    }
    property int type: Heading.Type.Normal

    Union.Element.type: "Heading"
    Union.Element.hints: [
        Union.ElementHint { name: "level-" + control.level },
        Union.ElementHint { name: "primary"; when: control.type === Heading.Primary },
        Union.ElementHint { name: "secondary"; when: control.type === Heading.Secondary },
    ]

    Accessible.role: Accessible.Heading

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding:  Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    font: Union.Style.properties.text.font ?? Application.font

    color: Union.Style.properties.text.color ?? palette.text

    background: Union.StyledRectangle { }
}

