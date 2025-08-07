/*
 *  SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *  SPDX-FileCopyrightText: 2018 Kai Uwe Broulik <kde@privat.broulik.de>
 *  SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.kirigami.templates as KT

import org.kde.union.impl as Union

KT.InlineMessage {
    id: control

    Union.Element.type: "InlineMessage"
    Union.Element.attributes: {
        let result = {}
        switch (control.type) {
        case Kirigami.MessageType.Positive:
            result.type = "positive"
            break
        case Kirigami.MessageType.Warning:
            result.type = "warning"
            break
        case Kirigami.MessageType.Error:
            result.type = "error"
            break
        }

        switch (control.position) {
        case KT.InlineMessage.Position.Inline:
            result.position = "inline"
            break
        case KT.InlineMessage.Position.Header:
            result.position = "header"
            break
        case KT.InlineMessage.Position.Footer:
            result.position = "footer"
            break
        }

        return result
    }

    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding:  Union.Style.properties.layout.padding.bottom

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    font: Union.Style.properties.text.font

    spacing: Union.Style.properties.layout.spacing

    icon {
        width: Union.Style.properties.icon.width ?? 0
        height: Union.Style.properties.icon.height ?? 0
        name: Union.Style.properties.icon.name ?? ""
        source: Union.Style.properties.icon.source ?? ""
    }

    background: Union.StyledRectangle { }
}
