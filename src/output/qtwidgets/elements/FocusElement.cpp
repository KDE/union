// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "FocusElement.h"
#include "UnionPainter.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

#include "SharedNames.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

FocusElement::FocusElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
{
    update(option);
}

FocusElement::~FocusElement()
{
}

void FocusElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    if (const auto newOpt = qstyleoption_cast<const QStyleOptionFocusRect *>(option)) {
        m_focusOption = newOpt;
    }
    if (m_styleOption) {
        updateGraphicsEffect(m_styleOption->rect);
    }
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

void FocusElement::drawBackground(QPainter *painter) const
{
    if (m_isValid && m_styleOption) {
        UnionPainter unionPainter(painter, m_elementProperties);
        unionPainter.setRect(m_styleOption->rect);
        if (m_focusOption) {
            unionPainter.setOverrideBackgroundColor(m_focusOption->backgroundColor);
        }
        unionPainter.draw();
    }
}

void FocusElement::layout()
{
    m_elementList = prepareElements(m_styleOption, m_widget, {ElementString::FocusFrame});
    if (!m_elementList.isEmpty()) {
        m_elementProperties = queryProperties(m_elementList);
        m_isValid = true;
    } else {
        m_isValid = false;
    }
}

Union::IdentifierList FocusElement::elementHints() const
{
    Union::IdentifierList hints;
    if (m_styleOption && m_styleOption->state.testFlag(QStyle::State_FocusAtBorder)) {
        hints.append(u"focus-at-border"_s);
    }
    return hints;
}

qreal FocusElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_FocusFrameVMargin:
        return averageVPadding();
    case QStyle::PM_FocusFrameHMargin:
        return averageHPadding();
    default:
        break;
    }
    return 0;
}

QString FocusElement::fallbackElementName([[maybe_unused]] const QStyleOption *opt) const
{
    return ElementString::FocusFrame;
}