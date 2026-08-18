// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "StyledRectangle.h"

#include "PixelAlignment.h"
#include "StyleRule.h"

#include "scenegraph/OutlineBorderRectangleNode.h"
#include "scenegraph/RectangleNode.h"
#include "scenegraph/RectangleShadowNode.h"

using namespace Union;
using namespace Union::Properties;
using namespace Union::Quick;
using namespace Qt::StringLiterals;

StyledRectangle::StyledRectangle(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents);
    setFlag(QQuickItem::ItemObservesViewport);
}

QuickStyle *StyledRectangle::style() const
{
    return m_style;
}

void StyledRectangle::setStyle(QuickStyle *newStyle)
{
    if (newStyle == m_style) {
        return;
    }

    m_style = newStyle;
    Q_EMIT styleChanged();
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

void StyledRectangle::itemChange(QQuickItem::ItemChange change, [[maybe_unused]] const QQuickItem::ItemChangeData &value)
{
    if (change == QQuickItem::ItemTransformHasChanged) {
        update();
    }
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

    // Ensure we *don't* delete the node at this point.
    guard.dismiss();

    return updateShaderNode(node, query->properties());
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

QSGNode *StyledRectangle::updateShaderNode(QSGNode *node, const StylePropertyGroup *style)
{
    if (!node) {
        node = new QSGNode{};
    }

    auto shadows = style->shadow() ? QList{style->shadow()} : QList<ShadowPropertyGroup *>{};
    auto backgrounds = style->background() ? QList{style->background()} : QList<BackgroundPropertyGroup *>{};
    auto border = style->border();
    auto outline = style->outline();
    auto borderOutlineEnabled = border || outline;

    auto expectedChildCount = shadows.size() + backgrounds.size() + (borderOutlineEnabled ? 1 : 0);
    auto shadowsChanged = shadows.size() != m_shadowCount;
    auto backgroundsChanged = backgrounds.size() != m_backgroundCount;

    if (node->childCount() != expectedChildCount || shadowsChanged || backgroundsChanged || borderOutlineEnabled != m_borderOutlineEnabled) {
        while (node->childCount() > 0) {
            auto childNode = node->firstChild();
            node->removeChildNode(childNode);
            delete childNode;
        }

        m_shadowCount = shadows.size();
        m_backgroundCount = backgrounds.size();
        m_borderOutlineEnabled = bool(border || outline);

        for (int i = 0; i < m_shadowCount; ++i) {
            auto shadowNode = new RectangleShadowNode{};
            shadowNode->setFlag(QSGNode::OwnedByParent, true);
            node->appendChildNode(shadowNode);
        }

        for (int i = 0; i < m_backgroundCount; ++i) {
            auto rectangleNode = new RectangleNode{};
            rectangleNode->setFlag(QSGNode::OwnedByParent, true);
            node->appendChildNode(rectangleNode);
        }

        if (m_borderOutlineEnabled) {
            auto borderNode = new OutlineBorderRectangleNode{};
            borderNode->setFlag(QSGNode::OwnedByParent, true);
            node->appendChildNode(borderNode);
        }
    }

    auto rect = alignRect(boundingRect(), this);
    auto cornerSizes = style->corners() ? style->corners()->radii() : CornersPropertyGroup::CornerRadii{};

    // Shader corner radius order is bottom right, top right, bottom left, top left.
    auto radii = QVector4D{float(cornerSizes.bottomRight), float(cornerSizes.topRight), float(cornerSizes.bottomLeft), float(cornerSizes.topLeft)};

    int i = 0;
    for (; i < m_shadowCount; ++i) {
        auto shadow = shadows.at(i);
        auto shadowNode = static_cast<RectangleShadowNode *>(node->childAtIndex(i));
        shadowNode->setItemRect(rect);
        shadowNode->setRadius(radii);
        shadowNode->setBlur(shadow->blur().value_or(0.0));
        shadowNode->setSpread(shadow->size().value_or(0.0));
        shadowNode->setOffset(shadow->offset() ? shadow->offset()->toVector2D() : QVector2D{});
        shadowNode->setColor(shadow->color().value_or(Union::Color{}).toQColor());
        shadowNode->update();
    }

    for (; i < m_shadowCount + m_backgroundCount; ++i) {
        auto background = backgrounds.at(i - m_shadowCount);
        auto backgroundNode = static_cast<RectangleNode *>(node->childAtIndex(i));
        backgroundNode->setRect(rect);
        backgroundNode->setRadius(radii);
        backgroundNode->setColor(background->color().value_or(Color{}).toQColor());
        backgroundNode->setImage(background->image());
        backgroundNode->setWindow(window());
        backgroundNode->update();
    }

    if (m_borderOutlineEnabled) {
        auto borderNode = static_cast<OutlineBorderRectangleNode *>(node->childAtIndex(i));
        borderNode->m_itemRect = rect;
        borderNode->m_border = style->border();
        borderNode->m_outline = style->outline();
        borderNode->m_radius = radii;
        borderNode->m_window = window();
        borderNode->update();
    }

    return node;
}

#include "moc_StyledRectangle.cpp"
