// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "LineNode.h"

#include <QSGSimpleRectNode>
#include <QSGSimpleTextureNode>

#include <Color.h>

LineNode::LineNode()
{
}

void LineNode::update(QQuickWindow *window)
{
    auto colorNode = ensureChildNode<QSGSimpleRectNode>(0);
    colorNode->setColor(line.color().value_or(Union::Color{}).toQColor());
    colorNode->setRect(rect);

    QRectF sourceRect;
    QSGTexture *texture = createTextureForImageProperty(window, line.image(), rect, sourceRect);
    if (texture) {
        auto textureNode = ensureChildNode<QSGSimpleTextureNode>(1);
        textureNode->setOwnsTexture(true);
        textureNode->setTexture(texture);
        textureNode->setSourceRect(sourceRect);
        textureNode->setRect(rect);
    } else {
        ensureChildRemoved(1);
    }
}
