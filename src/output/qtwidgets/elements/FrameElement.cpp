// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "FrameElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

FrameElement::FrameElement(const QStyleOptionFrame *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_frameOption(option)
{
    update();
}

FrameElement::~FrameElement()
{
}

void FrameElement::update()
{
    updateSubElementList();
    layout();
}

void FrameElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }
    switch (enums.ControlElement) {
    case QStyle::CE_FocusFrame:
    case QStyle::CE_ShapedFrame:
        drawBackground(painter);
        break;
    }
    switch (enums.PrimitiveElement) {
    case QStyle::PE_FrameDockWidget:
    case QStyle::PE_FrameWindow:
    case QStyle::PE_Frame:
        drawFrame(painter);
        break;
    }
}

void FrameElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(ElementString::Frame);
}

QStringList FrameElement::elementHints() const
{
    return frameHints(m_frameOption);
}

qreal FrameElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_DefaultFrameWidth:
        return averageBorderSize();
    default:
        break;
    }
    return 0;
}
