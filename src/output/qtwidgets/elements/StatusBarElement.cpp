// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "StatusBarElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

StatusBarElement::StatusBarElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
{
    update(option);
}

StatusBarElement::~StatusBarElement()
{
}

void StatusBarElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    updateSubElementList();
    layout();
}

void StatusBarElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }
    switch (enums.PrimitiveElement) {
    case QStyle::PE_PanelStatusBar:
        drawBackground(painter);
        break;
    case QStyle::PE_FrameStatusBarItem:
        drawItem(painter);
        break;
    }
}

void StatusBarElement::updateSubElementList()
{
    m_subElementList = {ElementString::Item};
}

void StatusBarElement::layout()
{
    // Background and content (items inside statusbar) is separate
    m_elementList = prepareElements(&m_styleOption, m_widget);
    if (!m_elementList.isEmpty()) {
        m_elementProperties = queryProperties(m_elementList);
        m_layoutMap = layoutMap(&m_styleOption, m_subElementList);
    }

    m_contentElementList = prepareElements(&m_styleOption, m_widget, m_subElementList);
    if (!m_contentElementList.isEmpty()) {
        m_contentProperties = queryProperties(m_contentElementList);
        m_isValid = true;
    } else {
        m_isValid = false;
        qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
    }
}

void StatusBarElement::drawItem(QPainter *painter) const
{
    drawBackgroundRectangle(painter, m_styleOption.rect, m_contentProperties);
}

QString StatusBarElement::fallbackElementName(const QStyleOption *opt) const
{
    Q_UNUSED(opt);
    return ElementString::StatusBar;
}