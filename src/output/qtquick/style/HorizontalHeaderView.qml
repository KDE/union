// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T

import org.kde.union.impl as Union

T.HorizontalHeaderView {
    id: control
    Union.Element.type: "HeaderView"
    Union.Element.states {
        activeFocus: control.activeFocus
        enabled: control.enabled
    }
    Union.Element.hints: Union.ElementHint { name: "horizontal" }

    implicitWidth: syncView ? syncView.width : 0
    implicitHeight: Math.max(1, contentHeight)

    delegate: HorizontalHeaderViewDelegate {}
}
