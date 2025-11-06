// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2017 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Niccolò Venerandi <niccolo@venerandi.com>

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import org.kde.kirigami as Kirigami

import org.kde.union.impl as Union

T.Dialog {
    id: control

    Union.Element.type: "Dialog"

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Math.ceil(contentWidth) + leftPadding + rightPadding,
                            implicitHeaderWidth,
                            implicitFooterWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             Math.ceil(contentHeight) + topPadding + bottomPadding
                             + (implicitHeaderHeight > 0 ? implicitHeaderHeight + spacing : 0)
                             + (implicitFooterHeight > 0 ? implicitFooterHeight + spacing : 0))

    spacing: Union.Style.properties.layout.spacing
    topPadding: Union.Style.properties.layout.padding.top
    bottomPadding: Union.Style.properties.layout.padding.bottom
    leftPadding: Union.Style.properties.layout.padding.left
    rightPadding: Union.Style.properties.layout.padding.right

    leftInset: Union.Style.properties.layout.inset.left
    rightInset: Union.Style.properties.layout.inset.right
    topInset: Union.Style.properties.layout.inset.top
    bottomInset: Union.Style.properties.layout.inset.bottom

    background: Union.StyledRectangle { }

    header: Label {
        Union.Element.hints: ["header"]
        text: control.title
        visible: control.title
        font: Union.Style.properties.text.font
        topPadding: Union.Style.properties.layout.padding.top
        bottomPadding: Union.Style.properties.layout.padding.bottom
        leftPadding: Union.Style.properties.layout.padding.left
        rightPadding: Union.Style.properties.layout.padding.right
    }

    footer: DialogButtonBox {
        visible: count > 0
    }
}
