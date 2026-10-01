// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "FocusElement.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

#include "SharedNames.h"

using namespace Qt::StringLiterals;

FocusElement::FocusElement(const QStyleOptionFocusRect *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_focusOption(option)
{
    update();
}

FocusElement::~FocusElement()
{
}

void FocusElement::update()
{
    layout();
}

void FocusElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }
    switch (enums.PrimitiveElement) {
    case QStyle::PE_FrameFocusRect:
        drawBackground(painter);
        break;
    }
}

void FocusElement::layout()
{
    m_elementList = prepareElements(m_styleOption, m_widget, {ElementString::FocusFrame});
    if (!m_elementList.isEmpty()) {
        m_elementProperties = queryProperties(m_elementList);
    }
}

QStringList FocusElement::elementHints() const
{
    QStringList hints;
    if (m_styleOption && m_focusOption->state.testFlag(QStyle::State_FocusAtBorder)) {
        hints.append(u"focus-at-border"_s);
    }
    return hints;
}

qreal FocusElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_FocusFrameVMargin:
        return averageVPadding(m_elementProperties);
    case QStyle::PM_FocusFrameHMargin:
        return averageHPadding(m_elementProperties);
    default:
        break;
    }
    return 0;
}
