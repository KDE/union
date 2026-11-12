// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "SizeGripElement.h"

#include "SharedNames.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

SizeGripElement::SizeGripElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
{
    update(option);
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

void SizeGripElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    if (const auto newOpt = qstyleoption_cast<const QStyleOptionSizeGrip *>(option)) {
        m_sizeGripOption = newOpt;
    }
    layout();
}

void SizeGripElement::layout()
{
    m_elementList = prepareElements(m_styleOption, m_widget, {ElementString::SizeGrip});
    if (!m_elementList.isEmpty()) {
        m_elementProperties = queryProperties(m_elementList);
        m_isValid = true;
    } else {
        m_isValid = false;
    }
}

qreal SizeGripElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_SizeGripSize:
        return width();
    default:
        break;
    }
    return 0;
}

QString SizeGripElement::fallbackElementName([[maybe_unused]] const QStyleOption *opt) const
{
    return ElementString::SizeGrip;
}
