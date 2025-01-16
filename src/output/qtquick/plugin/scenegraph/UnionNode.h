// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QQuickWindow>
#include <QSGNode>

#include <properties/ImageProperty.h>

/**
 * A convenience base class that contains some things that are useful for all node implementations.
 */
class UnionNode : public QSGNode
{
public:
    QRectF rect;

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
    QSGTexture *createTextureForImageProperty(QQuickWindow *window,
                                              const std::optional<Union::Properties::ImageProperty> &property,
                                              const QRectF &destinationRect,
                                              QRectF &sourceRect);
};

/**
 * A node that can be used as a placeholder when child indices need to be maintained.
 */
class PlaceholderNode : public QSGNode
{
    // This node intentionally does nothing.
};

/**
 * A convenience base class for nodes that consist of several child nodes.
 */
class HierarchyNode : public UnionNode
{
public:
    HierarchyNode();

    /**
     * Ensure the hierarchy is built.
     *
     * This will call buildNodeHierarchy() if needed, either when the node was
     * just created or when rebuildHierarchy() was called. If the hierarchy
     * needs to be rebuilt, it will first clean up the existing children.
     */
    void ensureHierarchy();

    /**
     * Trigger a rebuild of the hierarchy the next time ensureHierachy() is called.
     */
    void rebuildHierarchy();

protected:
    /**
     * This should be reimplemented by subclasses to build the node hierarchy.
     *
     * The node will not have any existing children when this gets called.
     */
    virtual void buildNodeHierarchy() = 0;

private:
    bool m_rebuildNodeHierarchy = true;
};
