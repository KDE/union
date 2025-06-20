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

inline QRectF adjustRectForShadow(const QRectF &rect, float shadowSize, const QVector2D &offsets, const QVector2D &aspect)
{
    auto adjusted = rect.adjusted(-shadowSize * aspect.x(), -shadowSize * aspect.y(), shadowSize * aspect.x(), shadowSize * aspect.y());
    auto offsetLength = offsets.length();
    adjusted.adjust(-offsetLength * aspect.x(), -offsetLength * aspect.y(), offsetLength * aspect.x(), offsetLength * aspect.y());
    return adjusted;
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

    // Ensure we *don't* delete the node at this point.
    guard.dismiss();

    if (background.image().value_or(ImageProperty{}).hasAnyValue()) {
        return updateRectangleNode(node, background);
    } else {
        return updateShaderNode(node, background);
    }

    return node;
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

QSGNode *StyledRectangle::updateRectangleNode(QSGNode *node, const Union::Properties::BackgroundProperty &background)
{
    if (!node) {
        node = new RectangleNode{};
    }

    auto rectangleNode = static_cast<RectangleNode *>(node);
    rectangleNode->setRectangle(background);
    rectangleNode->rect = boundingRect();
    rectangleNode->update(window());

    return rectangleNode;
}

QSGNode *StyledRectangle::updateShaderNode(QSGNode *node, const BackgroundProperty &background)
{
    if (!node) {
        node = new ShaderNode{};
    }

    auto shaderNode = static_cast<ShaderNode *>(node);

    if (background.border().has_value()) {
        shaderNode->setShader(u"shadowed_border_rectangle"_s);
    } else {
        shaderNode->setShader(u"shadowed_rectangle"_s);
    }

    auto rect = boundingRect();

    shaderNode->setRect(rect);
    shaderNode->setUniformBufferSize(sizeof(float) * 40);

    auto aspect = calculateAspect(rect);
    auto minDimension = float(std::min(rect.width(), rect.height()));

    auto borderLeft = background.border().value_or(BorderProperty{}).left().value_or(LineProperty{});

    auto shadow = background.shadow().value_or(ShadowProperty{});
    auto shadowSize = shadow.size().value_or(0.0);

    auto offsets = shadow.offset().value_or(OffsetProperty{});
    auto offset = QVector2D{float(offsets.horizontal().value_or(0.0)), float(offsets.vertical().value_or(0.0))};

    shaderNode->setRect(adjustRectForShadow(rect, shadowSize, offset, aspect));

    auto corners = background.corners().value_or(CornersProperty{});
    QVector4D radii;
    radii.setX(corners.topLeft().value_or(CornerProperty{}).radius().value_or(0.0));
    radii.setY(corners.topRight().value_or(CornerProperty{}).radius().value_or(0.0));
    radii.setZ(corners.bottomLeft().value_or(CornerProperty{}).radius().value_or(0.0));
    radii.setW(corners.bottomRight().value_or(CornerProperty{}).radius().value_or(0.0));

    UniformDataStream stream(shaderNode->uniformData());
    stream.skipMatrixOpacity();
    stream << float(shadowSize / minDimension) * 2.0f // size
           << float(borderLeft.size().value_or(0.0)) / minDimension // border_width
           << aspect // aspect
           << offset / minDimension // offset
           << radii / minDimension // radius
           << ShaderNode::toPremultiplied(background.color().value_or(Qt::transparent)) // color
           << ShaderNode::toPremultiplied(shadow.color().value_or(Qt::transparent)) // shadow_color
           << ShaderNode::toPremultiplied(borderLeft.color().value_or(Qt::transparent)); // border_color
    shaderNode->markDirty(QSGNode::DirtyMaterial);

    shaderNode->update();

    return shaderNode;
}
