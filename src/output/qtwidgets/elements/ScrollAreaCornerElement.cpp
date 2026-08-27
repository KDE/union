// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ScrollAreaCornerElement.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

#include "SharedNames.h"

using namespace Qt::StringLiterals;

ScrollAreaCornerElement::ScrollAreaCornerElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_cornerOption(option)
{
    update();
}

ScrollAreaCornerElement::~ScrollAreaCornerElement()
{
}

void ScrollAreaCornerElement::update()
{
    layout();
}

void ScrollAreaCornerElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }
    switch (enums.PrimitiveElement) {
    case QStyle::PE_PanelScrollAreaCorner:
        drawBackground(painter);
        break;
    }
}

void ScrollAreaCornerElement::layout()
{
    // Background and content is separate
    m_backgroundElementList = prepareElements(m_cornerOption, m_widget, {ElementString::ScrollAreaCorner});
    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
    }
}
