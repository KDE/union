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
    Union.Element.hints: {
        let result = ["level-" + control.level]
        switch (control.type) {
        case Heading.Primary:
            result.push("primary")
            break
        case Heading.Secondary:
            result.push("secondary")
            break
        default:
            break
        }
        return result
    }

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
    palette: Union.Style.properties.palette.quickPalette

    background: Union.StyledRectangle { }
}

