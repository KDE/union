/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Background.h"

#include <QQuickWindow>
#include <QSGNinePatchNode>
#include <QSGSimpleRectNode>

#include "Style.h"
// #include "Style.h"
// #include "StyleElement.h"

using namespace Union;

Background::Background(QQuickItem *parent)
    : QQuickItem(parent)
{
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

QSGNode *Background::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *data)
{
    if (!node) {
        // node = window()->createNinePatchNode();
    }

    //     Q_UNUSED(data)
    //
    //     if (!node) {
    //         node = new QSGSimpleRectNode{};
    //     }
    //
    //     if (m_element->m_width > 0.0) {
    //         setImplicitWidth(m_element->m_width);
    //     }
    //     if (m_element->m_height > 0.0) {
    //         setImplicitHeight(m_element->m_height);
    //     }
    // //     setImplicitSize(m_element->m_width, m_element->m_height);
    //
    //     auto rectNode = static_cast<QSGSimpleRectNode*>(node);
    //
    //     rectNode->setRect(boundingRect());
    //     rectNode->setColor(m_element->m_backgroundColor);
    //
    //     return rectNode;
    return nullptr;
}
