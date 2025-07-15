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
    Union.Element.hints: ["level-" + control.level]
    Union.Element.attributes: {
        let result = {}
        switch (control.type) {
        case Heading.Normal:
            result.type = "normal"
        case Heading.Primary:
            result.type = "primary"
        case Heading.Secondary:
            result.type = "secondary"
        }
        return result
    }

    Accessible.role: Accessible.Heading

    leftPadding: Union.Positioner.padding.left
    rightPadding: Union.Positioner.padding.right
    topPadding: Union.Positioner.padding.top
    bottomPadding:  Union.Positioner.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    font: Union.Style.properties.text.font
    palette: Union.Style.properties.palette.quickPalette

    background: Union.StyledRectangle { }
}

