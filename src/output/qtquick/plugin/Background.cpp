/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Background.h"

#include <QQuickWindow>

#include "AreaNode.h"
#include "CornerNode.h"
#include "Element.h"
#include "LineNode.h"
#include "Style.h"

using namespace Union;

enum NodeElement {
    Center, //
    Left, //
    Right, //
    Top, //
    Bottom, //
    TopLeft, //
    TopRight, //
    BottomLeft, //
    BottomRight //
};

Background::Background(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents);
}

void Union::Background::componentComplete()
{
    QQuickItem::componentComplete();

    m_element = qobject_cast<QuickElement *>(qmlAttachedPropertiesObject<QuickElement>(this, true));
    connect(m_element, &QuickElement::updated, this, &Background::update);
}

QSGNode *Background::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData * /*data*/)
{
    if (!m_element) {
        return nullptr;
    }

    auto query = m_element->buildQuery();
    if (!query.execute()) {
        return nullptr;
    }

    if (!node) {
        node = new QSGNode();

        node->appendChildNode(new AreaNode());

        for (auto element [[maybe_unused]] : {Left, Right, Top, Bottom}) {
            node->appendChildNode(new LineNode{});
        }

        for (auto element [[maybe_unused]] : {TopLeft, TopRight, BottomLeft, BottomRight}) {
            node->appendChildNode(new CornerNode{});
        }
    }

    auto win = window();
    auto bounds = boundingRect();
    auto borderSizes = query.borderSizes();

    auto center = static_cast<AreaNode *>(node->childAtIndex(0));
    center->area = query.background().value_or(AreaDefinition());
    center->rect = bounds - borderSizes;
    center->update(win);

    if (query.border().has_value()) {
        auto borders = query.border().value();

        for (auto [position, definition] : std::initializer_list<std::pair<NodeElement, LineDefinition>>{
                 {NodeElement::Left, borders.left.value_or(LineDefinition{})},
                 {NodeElement::Right, borders.right.value_or(LineDefinition{})},
                 {NodeElement::Top, borders.top.value_or(LineDefinition{})},
                 {NodeElement::Bottom, borders.bottom.value_or(LineDefinition{})},
             }) {
            auto lineNode = static_cast<LineNode *>(node->childAtIndex(position));
            lineNode->line = definition;
            lineNode->margins = borderSizes;

            switch (position) {
            case NodeElement::Left:
                lineNode->rect = QRectF{0.0, borderSizes.top(), definition.size, bounds.height() - borderSizes.top() - borderSizes.bottom()};
                break;
            case NodeElement::Right:
                lineNode->rect = QRectF{bounds.width() - definition.size, //
                                        borderSizes.top(),
                                        definition.size,
                                        bounds.height() - borderSizes.top() - borderSizes.bottom()};
                break;
            case NodeElement::Top:
                lineNode->rect = QRectF{borderSizes.left(), 0.0, bounds.width() - borderSizes.left() - borderSizes.right(), definition.size};
                break;
            case NodeElement::Bottom:
                lineNode->rect = QRectF{borderSizes.left(), //
                                        bounds.height() - definition.size,
                                        bounds.width() - borderSizes.left() - borderSizes.right(),
                                        definition.size};
                break;
            default:
                break;
            }

            lineNode->update(win);
        }
    }

    if (query.corners().has_value()) {
        auto corners = query.corners().value();

        for (auto [position, definition] : std::initializer_list<std::pair<NodeElement, CornerDefinition>>{
                 {NodeElement::TopLeft, corners.topLeft.value_or(CornerDefinition{})},
                 {NodeElement::TopRight, corners.topRight.value_or(CornerDefinition{})},
                 {NodeElement::BottomLeft, corners.bottomLeft.value_or(CornerDefinition{})},
                 {NodeElement::BottomRight, corners.bottomRight.value_or(CornerDefinition{})},
             }) {
            auto cornerNode = static_cast<CornerNode *>(node->childAtIndex(position));
            cornerNode->corner = definition;

            switch (position) {
            case NodeElement::TopLeft:
                cornerNode->rect = QRectF{0.0, 0.0, borderSizes.left(), borderSizes.top()};
                break;
            case NodeElement::TopRight:
                cornerNode->rect = QRectF{bounds.width() - borderSizes.right(), 0.0, borderSizes.right(), borderSizes.top()};
                break;
            case NodeElement::BottomLeft:
                cornerNode->rect = QRectF{0.0, bounds.height() - borderSizes.bottom(), borderSizes.left(), borderSizes.bottom()};
                break;
            case NodeElement::BottomRight:
                cornerNode->rect = QRectF{bounds.width() - borderSizes.right(), //
                                          bounds.height() - borderSizes.bottom(),
                                          borderSizes.right(),
                                          borderSizes.bottom()};
                break;
            default:
                break;
            }

            cornerNode->update(win);
        }
    }

    return node;
}
