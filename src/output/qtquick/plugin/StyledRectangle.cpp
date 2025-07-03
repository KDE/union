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

inline float adjustmentForShadowOutline(float shadowSize, const QVector2D &offsets, const QVector4D &outlineSize)
{
    auto shadowAdjustment = shadowSize + offsets.length();
    auto maxOutline = std::max({outlineSize.x(), outlineSize.y(), outlineSize.z(), outlineSize.w()});
    return std::max(shadowAdjustment, maxOutline);
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

    if (style.border_or_new().left_or_new().image_or_new().hasAnyValue()) {
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

    auto shaderName = u"styledrectangle"_s;

    if (style.shadow().has_value()) {
        shaderName = u"-shadow"_s;
    }

    if (style.border().has_value()) {
        shaderName += u"-border"_s;
    }

    if (style.outline().has_value()) {
        shaderName += u"-outline"_s;
    }

    auto background = style.background_or_new();
    auto image = background.image_or_new().imageData();
    if (image.has_value()) {
        shaderName += u"-texture"_s;
    }

    shaderNode->setShader(shaderName);
    shaderNode->setUniformBufferSize(sizeof(float) * 52);

    if (image.has_value()) {
        shaderNode->setTextureChannels(1);
    }

    auto rect = boundingRect();

    shaderNode->setRect(rect);

    auto aspect = calculateAspect(rect);
    auto minDimension = float(std::min(rect.width(), rect.height()));

    auto border = style.border_or_new();
    QVector4D borderSize;
    borderSize.setX(border.left_or_new().size().value_or(0.0));
    borderSize.setY(border.top_or_new().size().value_or(0.0));
    borderSize.setZ(border.right_or_new().size().value_or(0.0));
    borderSize.setW(border.bottom_or_new().size().value_or(0.0));

    auto outline = style.outline_or_new();
    QVector4D outlineSize;
    outlineSize.setX(outline.left_or_new().size().value_or(0.0));
    outlineSize.setY(outline.top_or_new().size().value_or(0.0));
    outlineSize.setZ(outline.right_or_new().size().value_or(0.0));
    outlineSize.setW(outline.bottom_or_new().size().value_or(0.0));

    auto shadow = style.shadow_or_new();
    auto shadowSize = shadow.size().value_or(0.0);

    auto offsets = shadow.offset_or_new();
    auto offset = QVector2D{float(offsets.horizontal().value_or(0.0)), float(offsets.vertical().value_or(0.0))};

    auto adjustment = adjustmentForShadowOutline(shadowSize, offset, outlineSize);
    auto adjustedRect = rect.adjusted(-adjustment * aspect.x(), -adjustment * aspect.y(), adjustment * aspect.x(), adjustment * aspect.y());
    shaderNode->setRect(adjustedRect);

    auto corners = style.corners_or_new();
    QVector4D radii;
    radii.setX(corners.topRight_or_new().radius().value_or(0.0));
    radii.setY(corners.bottomRight_or_new().radius().value_or(0.0));
    radii.setZ(corners.topLeft_or_new().radius().value_or(0.0));
    radii.setW(corners.bottomLeft_or_new().radius().value_or(0.0));

    UniformDataStream stream(shaderNode->uniformData());
    stream.skipMatrixOpacity();
    stream << float(shadowSize / minDimension) // size
           << float(1.0 / (1.0 + (adjustment * 2.0 / minDimension))) // inverse_scale
           << borderSize / minDimension // border_width
           << outlineSize / minDimension // outline_width
           << aspect // aspect
           << offset / minDimension // offset
           << radii / minDimension // radius
           << ShaderNode::toPremultiplied(background.color().value_or(Qt::transparent)) // color
           << ShaderNode::toPremultiplied(shadow.color().value_or(Qt::transparent)) // shadow_color
           << ShaderNode::toPremultiplied(border.left_or_new().color().value_or(Qt::transparent)) // border_color
           << ShaderNode::toPremultiplied(outline.left_or_new().color().value_or(Qt::transparent)); // outline_color

    if (image.has_value()) {
        shaderNode->setTexture(0, image.value(), window());
    }

    shaderNode->markDirty(QSGNode::DirtyMaterial);

    shaderNode->update();

    return shaderNode;
}
