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
#include "ShadowNode.h"
#include "Style.h"

using namespace Union;

enum class NodeElement {
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
    ElementCount, //
};

Background::Background(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents);
}

QuickElement *Background::element() const
{
    return m_element;
}

void Background::setElement(QuickElement *element)
{
    if (element == m_element) {
        return;
    }

    if (m_element) {
        disconnect(m_element, &QuickElement::updated, this, &Background::update);
    }

    m_element = element;

    if (m_element) {
        connect(m_element, &QuickElement::updated, this, &Background::update);
    }

    Q_EMIT elementChanged();
    update();
}

void Background::componentComplete()
{
    QQuickItem::componentComplete();

    if (!m_element) {
        setElement(qobject_cast<QuickElement *>(qmlAttachedPropertiesObject<QuickElement>(this, true)));
    }
}

QSGNode *Background::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData * /*data*/)
{
    if (!m_element) {
        return nullptr;
    }

    auto query = m_element->query();
    if (!query.result()) {
        return nullptr;
    }

    if (!node) {
        node = new QSGNode();

        node->appendChildNode(new ShadowNode());

        node->appendChildNode(new AreaNode());

        for (auto element [[maybe_unused]] : {NodeElement::Left, NodeElement::Right, NodeElement::Top, NodeElement::Bottom}) {
            node->appendChildNode(new LineNode{});
        }

        for (auto element [[maybe_unused]] : {NodeElement::TopLeft, NodeElement::TopRight, NodeElement::BottomLeft, NodeElement::BottomRight}) {
            node->appendChildNode(new CornerNode{});
        }
    }

    Q_ASSERT(node->childCount() == int(NodeElement::ElementCount));

    auto win = window();
    auto margins = query.margins().value_or(SizeDefinition{}).toMargins();
    auto bounds = boundingRect();
    auto borderSizes = query.borderSizes();

    auto shadow = static_cast<ShadowNode *>(node->childAtIndex(int(NodeElement::Shadow)));
    shadow->shadow = query.shadow().value_or(ShadowDefinition{});
    shadow->rect = bounds + margins;
    shadow->update(win);

    auto center = static_cast<AreaNode *>(node->childAtIndex(int(NodeElement::Center)));
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
            auto lineNode = static_cast<LineNode *>(node->childAtIndex(int(position)));
            lineNode->line = definition;

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
            auto cornerNode = static_cast<CornerNode *>(node->childAtIndex(int(position)));
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
