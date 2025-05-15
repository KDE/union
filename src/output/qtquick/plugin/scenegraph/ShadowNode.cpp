// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "ShadowNode.h"

#include <properties/CornerProperty.h>
#include <properties/LineProperty.h>

#include "CornerNode.h"
#include "LineNode.h"

using namespace Union;
using namespace Union::Properties;

enum class SubNodeIndex {
    Left,
    Right,
    Top,
    Bottom,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    SubNodeCount,
};

ShadowNode::ShadowNode()
{
}

Union::Properties::ShadowProperty ShadowNode::shadow() const
{
    return m_shadow;
}

void ShadowNode::setShadow(const ShadowProperty &newShadow)
{
    if (newShadow == m_shadow) {
        return;
    }

    m_shadow = newShadow;
    rebuildHierarchy();
}

void ShadowNode::update(QQuickWindow *window)
{
    ensureHierarchy();

    if (!m_shadow.hasAnyValue()) {
        return;
    }

    QMarginsF offsets;
    if (m_shadow.offset().has_value()) {
        auto offsetProperty = m_shadow.offset().value();
        auto horizontal = offsetProperty.horizontal().value_or(0.0);
        auto vertical = offsetProperty.vertical().value_or(0.0);

        offsets.setLeft(horizontal < 0.0 ? std::abs(horizontal) : 0.0);
        offsets.setRight(horizontal > 0.0 ? horizontal : 0.0);
        offsets.setTop(vertical < 0.0 ? std::abs(vertical) : 0.0);
        offsets.setBottom(vertical > 0.0 ? vertical : 0.0);
    }

    auto offsetRect = QRectF{rect.x() - offsets.left(),
                             rect.y() - offsets.top(),
                             rect.width() + offsets.left() + offsets.right(),
                             rect.height() + offsets.top() + offsets.bottom()};

    QMarginsF margins;
    margins.setLeft(m_shadow.left().value_or(LineProperty{}).size().value_or(0.0));
    margins.setRight(m_shadow.right().value_or(LineProperty{}).size().value_or(0.0));
    margins.setTop(m_shadow.top().value_or(LineProperty{}).size().value_or(0.0));
    margins.setBottom(m_shadow.bottom().value_or(LineProperty{}).size().value_or(0.0));

    for (auto [subNode, property] : std::initializer_list<std::pair<SubNodeIndex, LineProperty>>{
             {SubNodeIndex::Left, m_shadow.left().value_or(LineProperty{})},
             {SubNodeIndex::Right, m_shadow.right().value_or(LineProperty{})},
             {SubNodeIndex::Top, m_shadow.top().value_or(LineProperty{})},
             {SubNodeIndex::Bottom, m_shadow.bottom().value_or(LineProperty{})},
         }) {
        auto node = static_cast<LineNode *>(childAtIndex(int(subNode)));

        QRectF lineRect;
        switch (subNode) {
        case SubNodeIndex::Left:
            lineRect = QRectF{offsetRect.left(), offsetRect.top() + margins.top(), margins.left(), offsetRect.height() - margins.top() - margins.bottom()};
            break;
        case SubNodeIndex::Right:
            lineRect = QRectF{offsetRect.right() - margins.right(),
                              offsetRect.top() + margins.top(),
                              margins.right(),
                              offsetRect.height() - margins.top() - margins.bottom()};
            break;
        case SubNodeIndex::Top:
            lineRect = QRectF(offsetRect.left() + margins.left(), offsetRect.top(), offsetRect.width() - margins.left() - margins.right(), margins.top());
            break;
        case SubNodeIndex::Bottom:
            lineRect = QRectF(offsetRect.left() + margins.left(),
                              offsetRect.bottom() - margins.bottom(),
                              offsetRect.width() - margins.left() - margins.right(),
                              margins.bottom());
            break;
        default:
            break;
        }

        if (node->line == property && node->rect == lineRect) {
            continue;
        }

        node->line = property;
        node->rect = lineRect;
        node->update(window);
    }

    for (auto [subNode, property] : std::initializer_list<std::pair<SubNodeIndex, CornerProperty>>{
             {SubNodeIndex::TopLeft, m_shadow.topLeft().value_or(CornerProperty{})},
             {SubNodeIndex::TopRight, m_shadow.topRight().value_or(CornerProperty{})},
             {SubNodeIndex::BottomLeft, m_shadow.bottomLeft().value_or(CornerProperty{})},
             {SubNodeIndex::BottomRight, m_shadow.bottomRight().value_or(CornerProperty{})},
         }) {
        auto node = static_cast<CornerNode *>(childAtIndex(int(subNode)));

        QRectF cornerRect;
        switch (subNode) {
        case SubNodeIndex::TopLeft:
            cornerRect = QRectF{offsetRect.left(), offsetRect.top(), margins.left(), margins.top()};
            break;
        case SubNodeIndex::TopRight:
            cornerRect = QRectF{offsetRect.right() - margins.right(), offsetRect.top(), margins.right(), margins.top()};
            break;
        case SubNodeIndex::BottomLeft:
            cornerRect = QRectF(offsetRect.left(), offsetRect.bottom() - margins.bottom(), margins.left(), margins.bottom());
            break;
        case SubNodeIndex::BottomRight:
            cornerRect = QRectF(offsetRect.right() - margins.right(), offsetRect.bottom() - margins.bottom(), margins.right(), margins.bottom());
            break;
        default:
            break;
        }

        if (node->corner == property && node->rect == cornerRect) {
            continue;
        }

        node->corner = property;
        node->rect = cornerRect;
        node->update(window);
    }
}

void ShadowNode::buildNodeHierarchy()
{
    if (!m_shadow.hasAnyValue()) {
        return;
    }

    for (auto subNode : {SubNodeIndex::Left, SubNodeIndex::Right, SubNodeIndex::Top, SubNodeIndex::Bottom}) {
        ensureChildNode<LineNode>(int(subNode));
    }

    for (auto subNode [[maybe_unused]] : {SubNodeIndex::TopLeft, SubNodeIndex::TopRight, SubNodeIndex::BottomLeft, SubNodeIndex::BottomRight}) {
        ensureChildNode<CornerNode>(int(subNode));
    }
}
