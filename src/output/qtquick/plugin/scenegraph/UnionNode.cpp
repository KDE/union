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

QSGTexture *UnionNode::createTextureForImageProperty(QQuickWindow *window,
                                                     const std::optional<Union::Properties::ImageProperty> &property,
                                                     const QRectF &destinationRect,
                                                     QRectF &sourceRect)
{
    if (!property || !property->imageData().has_value() || property->imageData().value().isNull()) {
        return nullptr;
    }

    auto image = property.value();
    auto flags = image.flags().value_or(Union::Properties::ImageFlags{});

    auto texture = window->createTextureFromImage(image.imageData().value(), QQuickWindow::TextureCanUseAtlas);
    texture->setHorizontalWrapMode(flags & Union::Properties::ImageFlag::RepeatX ? QSGTexture::Repeat : QSGTexture::ClampToEdge);
    texture->setVerticalWrapMode(flags & Union::Properties::ImageFlag::RepeatY ? QSGTexture::Repeat : QSGTexture::ClampToEdge);

    if (flags & Union::Properties::ImageFlag::StretchX) {
        sourceRect.setWidth(image.width().value_or(destinationRect.width()));
    } else {
        sourceRect.setWidth(destinationRect.width());
    }

    if (flags & Union::Properties::ImageFlag::StretchY) {
        sourceRect.setWidth(image.height().value_or(destinationRect.height()));
    } else {
        sourceRect.setWidth(destinationRect.height());
    }

    return texture;
}

HierarchyNode::HierarchyNode()
{
}

void HierarchyNode::ensureHierarchy()
{
    if (m_rebuildNodeHierarchy) {
        auto node = firstChild();
        while (node) {
            auto nextNode = node->nextSibling();
            delete node;
            node = nextNode;
        }
        removeAllChildNodes();

        buildNodeHierarchy();

        m_rebuildNodeHierarchy = false;
    }
}

void HierarchyNode::rebuildHierarchy()
{
    m_rebuildNodeHierarchy = true;
}
