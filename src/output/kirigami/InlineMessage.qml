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
    Union.Element.attributes: [
        Union.ElementAttribute {
            name: "type";
            value: switch (control.type) {
                case Kirigami.MessageType.Positive: return "positive"
                case Kirigami.MessageType.Warning: return "warning"
                case Kirigami.MessageType.Error: return "error"
                default: return undefined
            }
        },
        Union.ElementAttribute {
            name: "position"
            value: switch (control.position) {
                case KT.InlineMessage.Position.Inline: return "inline"
                case KT.InlineMessage.Position.Header: return "header"
                case KT.InlineMessage.Position.Footer: return "footer"
                default: return undefined
            }
        },
    ]

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
