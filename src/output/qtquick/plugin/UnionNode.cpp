/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "UnionNode.h"

#include <QSGTexture>
#include <QWindow>

void UnionNode::ensureChildRemoved(int index)
{
    if (index >= childCount()) {
        return;
    }

    auto node = childAtIndex(index);
    removeChildNode(node);
    delete node;
}

QSGTexture *UnionNode::createTextureForImageDefinition(QQuickWindow *window,
                                                       const std::optional<Union::ImageDefinition> &definition,
                                                       const QRectF &destinationRect,
                                                       QRectF &sourceRect)
{
    if (!definition) {
        return nullptr;
    }

    auto imageDefinition = definition.value();
    auto texture = window->createTextureFromImage(imageDefinition.imageData, QQuickWindow::TextureCanUseAtlas);
    texture->setHorizontalWrapMode(imageDefinition.flags & Union::ImageFlag::RepeatX ? QSGTexture::Repeat : QSGTexture::ClampToEdge);
    texture->setVerticalWrapMode(imageDefinition.flags & Union::ImageFlag::RepeatY ? QSGTexture::Repeat : QSGTexture::ClampToEdge);

    if (imageDefinition.flags & Union::ImageFlag::StretchX) {
        sourceRect.setWidth(imageDefinition.width);
    } else {
        sourceRect.setWidth(destinationRect.width());
    }

    if (imageDefinition.flags & Union::ImageFlag::StretchY) {
        sourceRect.setWidth(imageDefinition.height);
    } else {
        sourceRect.setWidth(destinationRect.height());
    }

    return texture;
}
