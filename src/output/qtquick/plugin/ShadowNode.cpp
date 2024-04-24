/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "ShadowNode.h"

#include <QSGSimpleRectNode>
#include <QSGSimpleTextureNode>

#include <Definition.h>

#include "CornerNode.h"
#include "LineNode.h"

using namespace Union;

enum class Element {
    Left,
    Right,
    Top,
    Bottom,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    ElementCount,
};

ShadowNode::ShadowNode()
{
}

void ShadowNode::update(QQuickWindow *window)
{
    if (childCount() < int(Element::ElementCount)) {
        for (auto element [[maybe_unused]] : {Element::Left, Element::Right, Element::Top, Element::Bottom}) {
            appendChildNode(new LineNode());
        }

        for (auto element [[maybe_unused]] : {Element::TopLeft, Element::TopRight, Element::BottomLeft, Element::BottomRight}) {
            appendChildNode(new CornerNode());
        }
    }

    QMarginsF offsets;
    if (shadow.offsets.has_value()) {
        offsets = shadow.offsets.value().toMargins();
    }

    auto offsetRect = QRectF{rect.x() + offsets.left(),
                             rect.y() + offsets.top(),
                             rect.width() + -offsets.left() + -offsets.right(),
                             rect.height() + -offsets.top() + -offsets.bottom()};

    QMarginsF margins;
    margins.setLeft(shadow.left.value_or(LineDefinition{}).size);
    margins.setRight(shadow.right.value_or(LineDefinition{}).size);
    margins.setTop(shadow.top.value_or(LineDefinition{}).size);
    margins.setBottom(shadow.bottom.value_or(LineDefinition{}).size);

    for (auto [element, definition] : std::initializer_list<std::pair<Element, LineDefinition>>{
             {Element::Left, shadow.left.value_or(LineDefinition{})},
             {Element::Right, shadow.right.value_or(LineDefinition{})},
             {Element::Top, shadow.top.value_or(LineDefinition{})},
             {Element::Bottom, shadow.bottom.value_or(LineDefinition{})},
         }) {
        auto node = static_cast<LineNode *>(childAtIndex(int(element)));

        QRectF lineRect;
        switch (element) {
        case Element::Left:
            lineRect = QRectF{offsetRect.left(), offsetRect.top() + margins.top(), margins.left(), offsetRect.height() - margins.top() - margins.bottom()};
            break;
        case Element::Right:
            lineRect = QRectF{offsetRect.right() - margins.right(),
                              offsetRect.top() + margins.top(),
                              margins.right(),
                              offsetRect.height() - margins.top() - margins.bottom()};
            break;
        case Element::Top:
            lineRect = QRectF(offsetRect.left() + margins.left(), offsetRect.top(), offsetRect.width() - margins.left() - margins.right(), margins.top());
            break;
        case Element::Bottom:
            lineRect = QRectF(offsetRect.left() + margins.left(),
                              offsetRect.bottom() - margins.bottom(),
                              offsetRect.width() - margins.left() - margins.right(),
                              margins.bottom());
            break;
        default:
            break;
        }

        if (node->line == definition && node->rect == lineRect) {
            continue;
        }

        node->line = definition;
        node->rect = lineRect;
        node->update(window);
    }

    for (auto [element, definition] : std::initializer_list<std::pair<Element, CornerDefinition>>{
             {Element::TopLeft, shadow.topLeft.value_or(CornerDefinition{})},
             {Element::TopRight, shadow.topRight.value_or(CornerDefinition{})},
             {Element::BottomLeft, shadow.bottomLeft.value_or(CornerDefinition{})},
             {Element::BottomRight, shadow.bottomRight.value_or(CornerDefinition{})},
         }) {
        auto node = static_cast<CornerNode *>(childAtIndex(int(element)));

        QRectF cornerRect;
        switch (element) {
        case Element::TopLeft:
            cornerRect = QRectF{offsetRect.left(), offsetRect.top(), margins.left(), margins.top()};
            break;
        case Element::TopRight:
            cornerRect = QRectF{offsetRect.right() - margins.right(), offsetRect.top(), margins.right(), margins.top()};
            break;
        case Element::BottomLeft:
            cornerRect = QRectF(offsetRect.left(), offsetRect.bottom() - margins.bottom(), margins.left(), margins.bottom());
            break;
        case Element::BottomRight:
            cornerRect = QRectF(offsetRect.right() - margins.right(), offsetRect.bottom() - margins.bottom(), margins.right(), margins.bottom());
            break;
        default:
            break;
        }

        if (node->corner == definition && node->rect == cornerRect) {
            continue;
        }

        node->corner = definition;
        node->rect = cornerRect;
        node->update(window);
    }
}
