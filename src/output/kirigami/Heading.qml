/*
 *  SPDX-FileCopyrightText: 2012 Sebastian Kügler <sebas@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union
import org.kde.kirigami.templates as KT

KT.Heading {
    id: control

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

    // Don't set the color as Headings in the plasma shell would
    // break otherwise. a proper solution involves own union style for plasmashell

    background: Union.StyledRectangle { }
}

