// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "SizeGripElement.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>
#include <qstyleoption.h>

#include "SharedNames.h"

using namespace Qt::StringLiterals;

SizeGripElement::SizeGripElement(const QStyleOptionSizeGrip *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_sizeGripOption(option)
{
    update();
}

SizeGripElement::~SizeGripElement()
{
}

void SizeGripElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_SizeGrip:
        drawBackground(painter);
        break;
    }
}

void SizeGripElement::update()
{
    layout();
}

void SizeGripElement::layout()
{
    m_elementList = prepareElements(m_styleOption, m_widget, {ElementString::SizeGrip});
    if (!m_elementList.isEmpty()) {
        m_elementProperties = queryProperties(m_elementList);
        m_isValid = true;
    }
}

qreal SizeGripElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_SizeGripSize:
        return width(m_elementProperties);
    default:
        break;
    }
    return 0;
}
