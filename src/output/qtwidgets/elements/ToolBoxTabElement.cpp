// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ToolBoxTabElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include "elements/AbstractElement.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>
#include <QTabBar>

using namespace Qt::StringLiterals;

ToolBoxTabElement::ToolBoxTabElement(const QStyleOptionToolBox *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_toolBoxOption(option)
{
    update();
}

ToolBoxTabElement::~ToolBoxTabElement()
{
}

void ToolBoxTabElement::update()
{
    setIcon(m_toolBoxOption->icon);
    setText(m_toolBoxOption->text);
    updateSubElementList();
    layout();
}

void ToolBoxTabElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_ToolBoxTabShape:
        drawBackground(painter);
        break;
    case QStyle::CE_ToolBoxTabLabel:
        drawIcon(painter);
        drawText(painter);
        break;
    case QStyle::CE_ToolBoxTab:
        drawBackground(painter);
        drawIcon(painter);
        drawText(painter);
        break;
    }
}

void ToolBoxTabElement::updateSubElementList()
{
    m_subElementList.clear();
    if (!m_toolBoxOption->icon.isNull()) {
        m_subElementList.append(ElementString::Icon);
    }
    if (!m_toolBoxOption->text.isEmpty()) {
        m_subElementList.append(ElementString::Text);
    }
}

void ToolBoxTabElement::layout()
{
    if (m_subElementList.isEmpty()) {
        m_isValid = false;
        return;
    }
    // Background and content is separate
    m_backgroundElementList = prepareElements(m_toolBoxOption, m_widget, {ElementString::ToolBox, ElementString::Tab});

    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
    }

    QStringList elements = {ElementString::ToolBox, ElementString::Tab};
    elements.append(m_subElementList);
    m_contentElementList = prepareElements(m_toolBoxOption, m_widget, elements);

    if (!m_contentElementList.isEmpty()) {
        m_contentProperties = queryProperties(m_contentElementList);
    }

    m_layoutMap = layoutMap(m_backgroundElementList, m_toolBoxOption, m_subElementList);
    m_isValid = true;
}

QRectF ToolBoxTabElement::subElementRect(QStyle::SubElement element) const
{
    if (!m_isValid) {
        return QRect();
    }

    if (element == QStyle::SE_ToolBoxTabContents) {
        QRectF unifiedRect;
        for (const auto &m : m_layoutMap) {
            unifiedRect = unifiedRect.united(m.rect.toRect());
        }
        unifiedRect.setSize(applyPaddingToSize(unifiedRect.size()));
    }
    return m_toolBoxOption->rect;
}
