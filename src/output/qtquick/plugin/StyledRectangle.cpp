/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "StyledRectangle.h"

#include <QQuickWindow>

#include "StyleRule.h"
#include "scenegraph/RectangleNode.h"

using namespace Union;

StyledRectangle::StyledRectangle(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents);
}

void StyledRectangle::componentComplete()
{
    QQuickItem::componentComplete();

    if (!m_style) {
        m_style = qobject_cast<QuickStyle *>(qmlAttachedPropertiesObject<QuickStyle>(this, true));
        // TODO: Make this some kind of watcher construction rather than needing a connection.
        connect(m_style, &QuickStyle::updated, this, &StyledRectangle::updateImplicitSize);
        updateImplicitSize();
    }
}

QSGNode *StyledRectangle::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData * /*data*/)
{
    if (!m_style) {
        return nullptr;
    }

    auto query = m_style->query();
    if (!query || !query->hasMatches()) {
        return nullptr;
    }

    if (!query->properties().background().has_value()) {
        return nullptr;
    }

    if (!node) {
        node = new RectangleNode{};
    }

    auto rectangleNode = static_cast<RectangleNode *>(node);
    rectangleNode->setRectangle(query->properties().background().value());
    rectangleNode->rect = boundingRect();
    rectangleNode->update(window());

    return rectangleNode;
}

void StyledRectangle::updateImplicitSize()
{
    if (!m_style) {
        return;
    }

    auto query = m_style->query();
    if (!query || !query->hasMatches()) {
        return;
    }

    if (query->properties().layout().has_value()) {
        auto layout = query->properties().layout().value();
        setImplicitSize(layout.width().value_or(0.0), layout.height().value_or(0.0));
    }

    update();
}
