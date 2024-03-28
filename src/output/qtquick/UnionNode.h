/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QQuickWindow>
#include <QSGNode>

#include <Definition.h>

/**
 * A convenience base class that contains some things that are useful for all node implementations.
 */
class UnionNode : public QSGNode
{
public:
    /**
     * Update the node, applying any pending changes.
     *
     * \param window The QQuickWindow this node is rendered in.
     */
    virtual void update(QQuickWindow *window) = 0;

    template<typename T>
    T *ensureChildNode(int index)
    {
        if (index == childCount()) {
            auto node = new T;
            appendChildNode(node);
            return node;
        } else if (index < childCount()) {
            return static_cast<T *>(childAtIndex(index));
        } else {
            return nullptr;
        }
    }

    void ensureChildRemoved(int index);
    QSGTexture *createTextureForImageDefinition(QQuickWindow *window,
                                                const std::optional<Union::ImageDefinition> &definition,
                                                const QRectF &destinationRect,
                                                QRectF &sourceRect);
};
