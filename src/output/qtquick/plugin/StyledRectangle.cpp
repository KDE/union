// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "StyledRectangle.h"

#include <QQuickWindow>

#include <properties/BackgroundProperty.h>
#include <properties/LineProperty.h>
#include <properties/ShadowProperty.h>

#include "StyleRule.h"

#include "scenegraph/RectangleNode.h"
#include "scenegraph/ShaderNode.h"
#include "scenegraph/UniformDataStream.h"

using namespace Union;
using namespace Union::Properties;
using namespace Qt::StringLiterals;

inline QVector2D calculateAspect(const QRectF &rect)
{
    auto aspect = QVector2D{1.0, 1.0};
    if (rect.width() >= rect.height()) {
        aspect.setX(rect.width() / rect.height());
    } else {
        aspect.setY(rect.height() / rect.width());
    }
    return aspect;
}

inline QRectF adjustRectForShadow(const QRectF &rect, float shadowSize, const QVector2D &offsets, float outlineSize, const QVector2D &aspect)
{
    auto offsetLength = offsets.length();
    auto adjustment = std::max(shadowSize + offsetLength, outlineSize);

    return rect.adjusted(-adjustment * aspect.x(), -adjustment * aspect.y(), adjustment * aspect.x(), adjustment * aspect.y());
}

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

    if (!query->properties().hasAnyValue()) {
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

    if (style.background_or_new().image_or_new().hasAnyValue()) {
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

    if (query->properties().layout().has_value()) {
        auto layout = query->properties().layout().value();
        setImplicitSize(layout.width().value_or(0.0), layout.height().value_or(0.0));
    }

    update();
}

QSizeF StyledRectangle::minimumSize(const Properties::StyleProperty &style)
{
    if (!style.corners() && !style.border()) {
        return QSizeF{0.0, 0.0};
    }

    auto corners = style.corners_or_new();
    auto border = style.border_or_new();

    auto left = std::initializer_list<qreal>{
        corners.topLeft_or_new().width().value_or(0.0),
        border.left_or_new().size().value_or(0.0),
        corners.bottomLeft_or_new().width().value_or(0.0),
    };
    auto right = std::initializer_list<qreal>{
        corners.topRight_or_new().width().value_or(0.0),
        border.right_or_new().size().value_or(0.0),
        corners.bottomRight_or_new().width().value_or(0.0),
    };
    auto top = std::initializer_list<qreal>{
        corners.topLeft_or_new().height().value_or(0.0),
        border.top_or_new().size().value_or(0.0),
        corners.topRight_or_new().height().value_or(0.0),
    };
    auto bottom = std::initializer_list<qreal>{
        corners.bottomLeft_or_new().height().value_or(0.0),
        border.bottom_or_new().size().value_or(0.0),
        corners.bottomRight_or_new().height().value_or(0.0),
    };

    return QSizeF{std::max(left) + std::max(right), std::max(top) + std::max(bottom)};
}

QSGNode *StyledRectangle::updateRectangleNode(QSGNode *node, const Union::Properties::StyleProperty &style)
{
    if (!node) {
        node = new RectangleNode{};
    }

    auto rectangleNode = static_cast<RectangleNode *>(node);
    rectangleNode->setStyle(style);
    rectangleNode->rect = boundingRect();
    rectangleNode->update(window());

    return rectangleNode;
}

QSGNode *StyledRectangle::updateShaderNode(QSGNode *node, const StyleProperty &style)
{
    if (!node) {
        node = new ShaderNode{};
    }

    auto shaderNode = static_cast<ShaderNode *>(node);

    auto shaderName = u"shadowed_"_s;

    if (style.outline().has_value()) {
        shaderName += u"outline_"_s;
    }

    if (style.border().has_value()) {
        shaderName += u"border_"_s;
    }

    shaderName += u"rectangle"_s;

    shaderNode->setShader(shaderName);

    auto rect = boundingRect();

    shaderNode->setRect(rect);
    shaderNode->setUniformBufferSize(sizeof(float) * 44);

    auto aspect = calculateAspect(rect);
    auto minDimension = float(std::min(rect.width(), rect.height()));

    auto borderLeft = style.border_or_new().left_or_new();
    auto outlineLeft = style.outline_or_new().left_or_new();

    auto shadow = style.shadow_or_new();
    auto shadowSize = shadow.size().value_or(0.0);

    auto offsets = shadow.offset_or_new();
    auto offset = QVector2D{float(offsets.horizontal().value_or(0.0)), float(offsets.vertical().value_or(0.0))};

    shaderNode->setRect(adjustRectForShadow(rect, shadowSize, offset, outlineLeft.size().value_or(0.0), aspect));

    auto corners = style.corners_or_new();
    QVector4D radii;
    radii.setX(corners.topRight_or_new().radius().value_or(0.0));
    radii.setY(corners.bottomRight_or_new().radius().value_or(0.0));
    radii.setZ(corners.topLeft_or_new().radius().value_or(0.0));
    radii.setW(corners.bottomLeft_or_new().radius().value_or(0.0));

    UniformDataStream stream(shaderNode->uniformData());
    stream.skipMatrixOpacity();
    stream << float(shadowSize / minDimension) * 2.0f // size
           << float(borderLeft.size().value_or(0.0)) / minDimension // border_width
           << float(outlineLeft.size().value_or(0.0)) / minDimension // outline_width
           << aspect // aspect
           << offset / minDimension // offset
           << radii / minDimension // radius
           << ShaderNode::toPremultiplied(style.background_or_new().color().value_or(Qt::transparent)) // color
           << ShaderNode::toPremultiplied(shadow.color().value_or(Qt::transparent)) // shadow_color
           << ShaderNode::toPremultiplied(borderLeft.color().value_or(Qt::transparent)) // border_color
           << ShaderNode::toPremultiplied(outlineLeft.color().value_or(Qt::transparent)); // outline_color

    shaderNode->markDirty(QSGNode::DirtyMaterial);

    shaderNode->update();

    return shaderNode;
}
