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
        updateImplicitSize();
    }
}

bool StyledRectangle::event(QEvent *event)
{
    if (event->type() == QuickStyleUpdatedEvent::s_type) {
        updateImplicitSize();
        return true;
    }

    return QQuickItem::event(event);
}

QSGNode *StyledRectangle::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData * /*data*/)
{
    // Ensure we clean up the old node if conditions change so we no longer want
    // to render anything.
    auto guard = qScopeGuard([node]() {
        if (node) {
            delete node;
        }
    });

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

    auto rect = boundingRect();
    auto background = query->properties().background().value();
    auto minSize = minimumBackgroundSize(background);

    // If we cannot fit the background image into the required rect, just don't
    // display anything.'
    if (rect.width() < minSize.width() || rect.height() < minSize.height()) {
        return nullptr;
    }

    if (!node) {
        node = new RectangleNode{};
    }

    auto rectangleNode = static_cast<RectangleNode *>(node);
    rectangleNode->setRectangle(background);
    rectangleNode->rect = rect;
    rectangleNode->update(window());

    // Ensure we *don't* delete the node at this point.
    guard.dismiss();

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

QSizeF StyledRectangle::minimumBackgroundSize(const Properties::BackgroundProperty &background)
{
    if (!background.corners() && !background.border()) {
        return QSizeF{0.0, 0.0};
    }

    auto corners = background.corners().value_or(Properties::CornersProperty{});
    auto border = background.border().value_or(Properties::BorderProperty{});

    auto left = std::initializer_list<qreal>{
        corners.topLeft().value_or(Properties::CornerProperty{}).width().value_or(0.0),
        border.left().value_or(Properties::LineProperty{}).size().value_or(0.0),
        corners.bottomLeft().value_or(Properties::CornerProperty{}).width().value_or(0.0),
    };
    auto right = std::initializer_list<qreal>{
        corners.topRight().value_or(Properties::CornerProperty{}).width().value_or(0.0),
        border.right().value_or(Properties::LineProperty{}).size().value_or(0.0),
        corners.bottomRight().value_or(Properties::CornerProperty{}).width().value_or(0.0),
    };
    auto top = std::initializer_list<qreal>{
        corners.topLeft().value_or(Properties::CornerProperty{}).height().value_or(0.0),
        border.top().value_or(Properties::LineProperty{}).size().value_or(0.0),
        corners.topRight().value_or(Properties::CornerProperty{}).height().value_or(0.0),
    };
    auto bottom = std::initializer_list<qreal>{
        corners.bottomLeft().value_or(Properties::CornerProperty{}).height().value_or(0.0),
        border.bottom().value_or(Properties::LineProperty{}).size().value_or(0.0),
        corners.bottomRight().value_or(Properties::CornerProperty{}).height().value_or(0.0),
    };

    return QSizeF{std::max(left) + std::max(right), std::max(top) + std::max(bottom)};
}
