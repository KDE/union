/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick

import org.kde.kirigami.templates as KT

import org.kde.union.impl as Union

KT.AbstractApplicationHeader {
    id: root

    Union.Element.type: "ApplicationHeader"

    background: Union.StyledRectangle { }
}
