// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "StyledRectangle.h"

#include <QQuickWindow>

#include <properties/BackgroundProperty.h>
#include <properties/LineProperty.h>
#include <properties/ShadowProperty.h>

#include "StyleRule.h"

#include "scenegraph/OutlineBorderRectangleNode.h"
#include "scenegraph/RectangleNode.h"
#include "scenegraph/RectangleShadowNode.h"

using namespace Union;
using namespace Union::Properties;
using namespace Qt::StringLiterals;

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
        return false;
    }

    if (event->type() == QuickStyleColorsChangedEvent::s_type) {
        update();
        return false;
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

    if (!query->properties()->hasAnyValue()) {
        return nullptr;
    }

    auto rect = boundingRect();
    if (rect.isEmpty()) {
        return nullptr;
    }

    auto style = query->properties();
    auto minSize = minimumSize(style);

    // If we cannot fit the background image into the required rect, just don't
    // display anything.'
    if (rect.width() < minSize.width() || rect.height() < minSize.height()) {
        return nullptr;
    }

    // Ensure we *don't* delete the node at this point.
    guard.dismiss();

    if (style->border() && style->border()->left() && style->border()->left()->image() && style->border()->left()->image()->hasAnyValue()) {
        return updateRectangleNode(node, style);
    } else {
        return updateShaderNode(node, style);
    }
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

    if (query->properties()->layout()) {
        auto layout = query->properties()->layout();
        setImplicitSize(layout->width().value_or(0.0), layout->height().value_or(0.0));
    }

    update();
}

QSizeF StyledRectangle::minimumSize(const Properties::StyleProperty *style)
{
    auto corners = style->corners();
    auto border = style->border();

    if (!corners && !border) {
        return QSizeF{0.0, 0.0};
    }

    auto cornerSizes = corners ? corners->radii() : CornersProperty::CornerRadii{};
    auto borderSizes = border ? border->sizes() : QMarginsF{};

    auto left = std::initializer_list<qreal>{
        cornerSizes.topLeft,
        borderSizes.left(),
        cornerSizes.bottomLeft,
    };
    auto right = std::initializer_list<qreal>{
        cornerSizes.topRight,
        borderSizes.right(),
        cornerSizes.bottomRight,
    };
    auto top = std::initializer_list<qreal>{
        cornerSizes.topLeft,
        borderSizes.top(),
        cornerSizes.topRight,
    };
    auto bottom = std::initializer_list<qreal>{
        cornerSizes.bottomLeft,
        borderSizes.bottom(),
        cornerSizes.bottomRight,
    };

    return QSizeF{std::max(left) + std::max(right), std::max(top) + std::max(bottom)};
}

QSGNode *StyledRectangle::updateRectangleNode(QSGNode *node, const Union::Properties::StyleProperty *style)
{
    return nullptr;

    // TODO: Reimplement
    // if (!node) {
    //     node = new RectangleNode{};
    // }

    // auto rectangleNode = static_cast<RectangleNode *>(node);
    // rectangleNode->setStyle(style);
    // rectangleNode->rect = boundingRect();
    // rectangleNode->update(window());

    // return rectangleNode;
}

QSGNode *StyledRectangle::updateShaderNode(QSGNode *node, const StyleProperty *style)
{
    if (!node) {
        node = new QSGNode{};
    }

    auto rect = boundingRect();

    auto cornerSizes = style->corners() ? style->corners()->radii() : CornersProperty::CornerRadii{};

    // Shader corner radius order is bottom right, top right, bottom left, top left.
    auto radii = QVector4D{float(cornerSizes.bottomRight), float(cornerSizes.topRight), float(cornerSizes.bottomLeft), float(cornerSizes.topLeft)};

    OutlineBorderRectangleNode *borderNode = nullptr;

    // Render the shadow as a separate node, followed by the actual rectangle.
    // This is done because the shadow geometry can vary greatly from the
    // rectangle geometry and separating them into different nodes made the
    // whole thing a lot simpler.
    if (auto shadow = style->shadow(); shadow && !shadow->isEmpty()) {
        if (node->childCount() == 0) {
            node->appendChildNode(new RectangleShadowNode{});
            node->appendChildNode(new OutlineBorderRectangleNode{});
        } else if (node->childCount() == 1) {
            node->prependChildNode(new RectangleShadowNode{});
        }

        auto shadowNode = static_cast<RectangleShadowNode *>(node->firstChild());
        shadowNode->setItemRect(rect);
        shadowNode->setRadius(radii);
        shadowNode->setBlur(shadow->blur().value_or(0.0));
        shadowNode->setSpread(shadow->size().value_or(0.0));
        shadowNode->setOffset(shadow->offset() ? shadow->offset()->toVector2D() : QVector2D{});
        shadowNode->setColor(shadow->color().value_or(Union::Color{}).toQColor());
        shadowNode->update();

        borderNode = static_cast<OutlineBorderRectangleNode *>(node->lastChild());
    } else {
        if (node->childCount() == 0) {
            node->appendChildNode(new OutlineBorderRectangleNode{});
        } else if (node->childCount() > 1) {
            auto shadowNode = node->firstChild();
            node->removeChildNode(shadowNode);
            delete shadowNode;
        }

        borderNode = static_cast<OutlineBorderRectangleNode *>(node->firstChild());
    }

    borderNode->m_itemRect = rect;
    borderNode->m_background = style->background();
    borderNode->m_border = style->border();
    borderNode->m_outline = style->outline();
    borderNode->m_radius = radii;
    borderNode->m_window = window();
    borderNode->update();

    return node;
}
