/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Background.h"

#include <QQuickWindow>

#include "AreaNode.h"
#include "CornerNode.h"
#include "LineNode.h"
#include "Style.h"
#include "StyleElement.h"

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
    m_internalElement = std::make_unique<Element>();
}

Element *Background::element() const
{
    if (m_elementOverride) {
        return m_elementOverride;
    } else {
        return m_internalElement.get();
    }
}

void Background::setElement(Element *newElement)
{
    if (newElement == m_elementOverride) {
        return;
    }

    m_elementOverride = newElement;
    connect(m_elementOverride, &Element::stateChanged, this, &QQuickItem::update);
    setImplicitSize(m_elementOverride->implicitWidth(), m_elementOverride->implicitHeight());
    Q_EMIT elementChanged();
}

// QString Background::elementName() const
// {
//     return m_elementName;
// }
//
// void Background::setElementName(const QString &newElementName)
// {
//     if (newElementName == m_elementName) {
//         return;
//     }
//
//     m_elementName = newElementName;
//     m_element = Style::instance()->get(m_elementName);
//     m_activeElement = Style::instance()->get(m_elementName, QStringLiteral("active"));
//     m_focusElement = Style::instance()->get(m_elementName, QStringLiteral("focus"));
//     m_hoverElement = Style::instance()->get(m_elementName, QStringLiteral("hover"));
//
//     update();
//     Q_EMIT elementNameChanged();
// }

// StyleElement *Background::styleElement() const
// {
//     if (m_active && m_activeElement) {
//         return m_activeElement.get();
//     }
//     if (m_focus && m_focusElement) {
//         return m_focusElement.get();
//     }
//     if (m_hover && m_hoverElement) {
//         return m_hoverElement.get();
//     }
//
//     return m_element.get();
// }

// void Background::stateChanged(ElementState::StateChange change)
// {
//
// }

QSGNode *Background::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData * /*data*/)
{
    if (!element() || !element()->styleElement()) {
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

    auto e = element()->styleElement();
    auto win = window();
    auto bounds = boundingRect();
    auto borderSizes = e->borderSizes();

    auto center = static_cast<AreaNode *>(node->childAtIndex(0));
    center->area = e->background().value_or(AreaDefinition());
    center->rect = bounds - borderSizes;
    center->update(win);

    if (e->border().has_value()) {
        auto borders = e->border().value();

        for (auto [position, definition] : std::initializer_list<std::pair<NodeElement, LineDefinition>>{
                 {NodeElement::Left, borders.left.value_or(LineDefinition{})},
                 {NodeElement::Right, borders.right.value_or(LineDefinition{})},
                 {NodeElement::Top, borders.top.value_or(LineDefinition{})},
                 {NodeElement::Bottom, borders.bottom.value_or(LineDefinition{})},
             }) {
            auto lineNode = static_cast<LineNode *>(node->childAtIndex(position));
            lineNode->line = definition;
            lineNode->margins = e->borderSizes();

            switch (position) {
            case NodeElement::Left:
                lineNode->rect = QRectF{0.0, borderSizes.top(), definition.size.value(), bounds.height() - borderSizes.top() - borderSizes.bottom()};
                break;
            case NodeElement::Right:
                lineNode->rect = QRectF{bounds.width() - definition.size.value(),
                                        borderSizes.top(),
                                        definition.size.value(),
                                        bounds.height() - borderSizes.top() - borderSizes.bottom()};
                break;
            case NodeElement::Top:
                lineNode->rect = QRectF{borderSizes.left(), 0.0, bounds.width() - borderSizes.left() - borderSizes.right(), definition.size.value()};
                break;
            case NodeElement::Bottom:
                lineNode->rect = QRectF{borderSizes.left(),
                                        bounds.height() - definition.size.value(),
                                        bounds.width() - borderSizes.left() - borderSizes.right(),
                                        definition.size.value()};
                break;
            default:
                break;
            }

            lineNode->update(win);
        }
    }

    if (e->corners().has_value()) {
        auto corners = e->corners().value();

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
                cornerNode->rect = QRectF{bounds.width() - borderSizes.right(),
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
