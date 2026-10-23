// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "RubberBandElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

RubberBandElement::RubberBandElement(const QStyleOptionRubberBand *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
{
    update(option);
}

RubberBandElement::~RubberBandElement()
{
}

void RubberBandElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    m_rubberBandOption = updateStyleOption<QStyleOptionRubberBand>(option);
    layout();
}

void RubberBandElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_RubberBand:
        drawBackground(painter);
        break;
    }
}

void RubberBandElement::layout()
{
    m_elementList = prepareElements(&m_rubberBandOption, m_widget, {ElementString::RubberBand});
    if (!m_elementList.isEmpty()) {
        m_elementProperties = queryProperties(m_elementList);
        m_isValid = true;
    } else {
        m_isValid = false;
    }
}

QString RubberBandElement::fallbackElementName(const QStyleOption *opt) const
{
    Q_UNUSED(opt);
    return ElementString::RubberBand;
}