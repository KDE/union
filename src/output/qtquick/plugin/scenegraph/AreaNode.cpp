/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "AreaNode.h"

#include <QSGSimpleRectNode>
#include <QSGSimpleTextureNode>

AreaNode::AreaNode()
{
}

void AreaNode::update(QQuickWindow *window)
{
    auto colorNode = ensureChildNode<QSGSimpleRectNode>(0);
    colorNode->setColor(color);
    colorNode->setRect(rect);

    QRectF sourceRect;
    QSGTexture *texture = createTextureForImageProperty(window, image, rect, sourceRect);

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
