/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "LineNode.h"

#include <QSGSimpleRectNode>
#include <QSGSimpleTextureNode>

LineNode::LineNode()
{
}

void LineNode::update(QQuickWindow *window)
{
    auto colorNode = ensureChildNode<QSGSimpleRectNode>(0);
    colorNode->setColor(line.color);
    colorNode->setRect(rect);

    QRectF sourceRect;
    QSGTexture *texture = createTextureForImageDefinition(window, line.image, rect, sourceRect);
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
