// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ToolTipElement.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

#include "SharedNames.h"

using namespace Qt::StringLiterals;

ToolTipElement::ToolTipElement(const QStyleOptionFrame *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_frameOption(option)
{
    update();
}

ToolTipElement::~ToolTipElement()
{
}

void ToolTipElement::update()
{
    layout();
}

void ToolTipElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }
    switch (enums.PrimitiveElement) {
    case QStyle::PE_PanelTipLabel:
        drawBackground(painter);
        break;
    }
}

void ToolTipElement::layout()
{
    m_backgroundElementList = prepareElements(m_frameOption, m_widget, {ElementString::ToolTip});
    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
        m_isValid = true;
    }
}

QStringList ToolTipElement::elementHints() const
{
    return frameHints(m_frameOption);
}

qreal ToolTipElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ToolTipLabelFrameWidth:
        return averageBorderSize();
    default:
        break;
    }
    return 0;
}
