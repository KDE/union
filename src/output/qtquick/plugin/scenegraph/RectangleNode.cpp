// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "RectangleNode.h"

#include <QSGSimpleRectNode>
#include <QSGSimpleTextureNode>

#include "AreaNode.h"
#include "CornerNode.h"
#include "LineNode.h"
#include "ShadowNode.h"

using namespace Union;
using namespace Union::Properties;

enum class SubNodeIndex {
    Shadow, //
    Center, //
    Left, //
    Right, //
    Top, //
    Bottom, //
    TopLeft, //
    TopRight, //
    BottomLeft, //
    BottomRight, //
    SubNodeCount, //
};

RectangleNode::RectangleNode()
    : HierarchyNode()
{
}

Union::Properties::StyleProperty RectangleNode::style() const
{
    return m_style;
}

void RectangleNode::setStyle(const Union::Properties::StyleProperty &newStyle)
{
    if (newStyle == m_style) {
        return;
    }

    m_style = newStyle;
    rebuildHierarchy();
}

void RectangleNode::update(QQuickWindow *window)
{
    ensureHierarchy();

    if (!m_style.hasAnyValue()) {
        return;
    }

    QMarginsF borderSizes;
    if (m_style.border().has_value()) {
        auto border = m_style.border().value();
        borderSizes.setLeft(border.left_or_new().size().value_or(0.0));
        borderSizes.setRight(border.right_or_new().size().value_or(0.0));
        borderSizes.setTop(border.top_or_new().size().value_or(0.0));
        borderSizes.setBottom(border.bottom_or_new().size().value_or(0.0));
    }

    if (m_style.shadow().has_value()) {
        auto shadowNode = static_cast<ShadowNode *>(childAtIndex(int(SubNodeIndex::Shadow)));
        shadowNode->setShadow(m_style.shadow().value());
        shadowNode->rect = rect;
        shadowNode->update(window);
    }

    auto center = static_cast<AreaNode *>(childAtIndex(int(SubNodeIndex::Center)));
    auto background = m_style.background_or_new();
    center->color = background.color().value_or(Qt::transparent);
    center->image = background.image();
    center->rect = rect - borderSizes;
    center->update(window);

    if (m_style.border().has_value()) {
        auto borders = m_style.border().value();

        for (auto [subNode, property] : std::initializer_list<std::pair<SubNodeIndex, LineProperty>>{
                 {SubNodeIndex::Left, borders.left_or_new()},
                 {SubNodeIndex::Right, borders.right_or_new()},
                 {SubNodeIndex::Top, borders.top_or_new()},
                 {SubNodeIndex::Bottom, borders.bottom_or_new()},
             }) {
            auto lineNode = static_cast<LineNode *>(childAtIndex(int(subNode)));

            QRectF lineRect;
            switch (subNode) {
            case SubNodeIndex::Left:
                lineRect = QRectF{0.0, borderSizes.top(), property.size().value_or(0.0), rect.height() - borderSizes.top() - borderSizes.bottom()};
                break;
            case SubNodeIndex::Right:
                lineRect = QRectF{rect.width() - property.size().value_or(0.0),
                                  borderSizes.top(),
                                  property.size().value_or(0.0),
                                  rect.height() - borderSizes.top() - borderSizes.bottom()};
                break;
            case SubNodeIndex::Top:
                lineRect = QRectF{borderSizes.left(), 0.0, rect.width() - borderSizes.left() - borderSizes.right(), property.size().value_or(0.0)};
                break;
            case SubNodeIndex::Bottom:
                lineRect = QRectF{borderSizes.left(),
                                  rect.height() - property.size().value_or(0.0),
                                  rect.width() - borderSizes.left() - borderSizes.right(),
                                  property.size().value_or(0.0)};
                break;
            default:
                break;
            }

            if (lineNode->line == property && lineNode->rect == lineRect) {
                continue;
            }

            lineNode->line = property;
            lineNode->rect = lineRect;
            lineNode->update(window);
        }
    }

    if (m_style.corners().has_value()) {
        auto corners = m_style.corners().value();

        for (auto [position, property] : std::initializer_list<std::pair<SubNodeIndex, CornerProperty>>{
                 {SubNodeIndex::TopLeft, corners.topLeft_or_new()},
                 {SubNodeIndex::TopRight, corners.topRight_or_new()},
                 {SubNodeIndex::BottomLeft, corners.bottomLeft_or_new()},
                 {SubNodeIndex::BottomRight, corners.bottomRight_or_new()},
             }) {
            auto cornerNode = static_cast<CornerNode *>(childAtIndex(int(position)));

            QRectF cornerRect;
            switch (position) {
            case SubNodeIndex::TopLeft: {
                qreal width = property.width().value_or(borderSizes.left());
                qreal height = property.height().value_or(borderSizes.top());
                cornerRect = QRectF{0.0, 0.0, width, height};
                break;
            }
            case SubNodeIndex::TopRight: {
                qreal width = property.width().value_or(borderSizes.right());
                qreal height = property.height().value_or(borderSizes.top());
                cornerRect = QRectF{rect.width() - width, 0.0, width, height};
                break;
            }
            case SubNodeIndex::BottomLeft: {
                qreal width = property.width().value_or(borderSizes.left());
                qreal height = property.height().value_or(borderSizes.bottom());
                cornerRect = QRectF{0.0, rect.height() - height, width, height};
                break;
            }
            case SubNodeIndex::BottomRight: {
                qreal width = property.width().value_or(borderSizes.right());
                qreal height = property.height().value_or(borderSizes.bottom());
                cornerRect = QRectF{rect.width() - width, rect.height() - height, width, height};
                break;
            }
            default:
                break;
            }

            if (cornerNode->corner == property && cornerNode->rect == cornerRect) {
                continue;
            }

            cornerNode->corner = property;
            cornerNode->rect = cornerRect;
            cornerNode->update(window);
        }
    }
}

void RectangleNode::buildNodeHierarchy()
{
    if (!m_style.hasAnyValue()) {
        return;
    }

    if (m_style.shadow().has_value()) {
        ensureChildNode<ShadowNode>(int(SubNodeIndex::Shadow));
    } else {
        ensureChildNode<PlaceholderNode>(int(SubNodeIndex::Shadow));
    }

    ensureChildNode<AreaNode>(int(SubNodeIndex::Center));

    for (auto subNode : {SubNodeIndex::Left, SubNodeIndex::Right, SubNodeIndex::Top, SubNodeIndex::Bottom}) {
        ensureChildNode<LineNode>(int(subNode));
    }

    for (auto subNode : {SubNodeIndex::TopLeft, SubNodeIndex::TopRight, SubNodeIndex::BottomLeft, SubNodeIndex::BottomRight}) {
        ensureChildNode<CornerNode>(int(subNode));
    }

    Q_ASSERT(childCount() == int(SubNodeIndex::SubNodeCount));
}
