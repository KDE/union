// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "CornerNode.h"

#include <QSGSimpleRectNode>
#include <QSGSimpleTextureNode>

CornerNode::CornerNode()
{
}

void CornerNode::update(QQuickWindow *window)
{
    auto colorNode = ensureChildNode<QSGSimpleRectNode>(0);
    colorNode->setColor(corner.color().value_or(Qt::transparent));
    colorNode->setRect(rect);

    QRectF sourceRect;
    QSGTexture *texture = createTextureForImageProperty(window, corner.image(), rect, sourceRect);

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
