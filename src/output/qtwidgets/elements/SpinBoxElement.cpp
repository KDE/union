// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "SpinBoxElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>
#include <qstyle.h>

using namespace Qt::StringLiterals;

SpinBoxElement::SpinBoxElement(const QStyleOptionSpinBox *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_spinBoxOption(option)
    , m_hasButtons(true)
{
    update();
}

SpinBoxElement::~SpinBoxElement()
{
}

void SpinBoxElement::update()
{
    m_indicatorElementList = prepareElements(m_spinBoxOption, m_widget, {ElementString::Indicator});
    if (!m_indicatorElementList.isEmpty()) {
        m_indicatorProperties = queryProperties(m_indicatorElementList);
    }
    m_hasButtons = (m_spinBoxOption->buttonSymbols != QAbstractSpinBox::NoButtons);
    updateSubElementList();
    layout();
}

void SpinBoxElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ComplexControl) {
    case QStyle::CC_SpinBox:
        drawBackground(painter);
        // For spinbox we need to manually create the indicator buttons
        if (m_spinBoxOption->buttonSymbols != QAbstractSpinBox::NoButtons) {
            bool arrows = (m_spinBoxOption->buttonSymbols == QAbstractSpinBox::UpDownArrows);
            // Increase
            auto up = *m_spinBoxOption;
            up.rect = subControlRect(QStyle::SC_SpinBoxUp).toRect();
            m_style->drawPrimitive(arrows ? QStyle::PE_IndicatorSpinUp : QStyle::PE_IndicatorSpinPlus, &up, painter);
            // Decrease
            auto down = *m_spinBoxOption;
            down.rect = subControlRect(QStyle::SC_SpinBoxDown).toRect();
            m_style->drawPrimitive(arrows ? QStyle::PE_IndicatorSpinDown : QStyle::PE_IndicatorSpinMinus, &down, painter);
        }
        break;
    }

    switch (enums.PrimitiveElement) {
    case QStyle::PE_IndicatorSpinPlus:
    case QStyle::PE_IndicatorSpinMinus:
    case QStyle::PE_IndicatorSpinUp:
    case QStyle::PE_IndicatorSpinDown:
        drawSpinIndicator(painter, (QStyle::PrimitiveElement)enums.PrimitiveElement);
        break;
    }
}

void SpinBoxElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(ElementString::Indicator);
}

QSizeF SpinBoxElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    const int frameWidth = m_style->pixelMetric(QStyle::PM_SpinBoxFrameWidth, m_spinBoxOption, m_widget);
    auto size = contentsSizeFromStyle;
    size += QSize(2 * frameWidth, 2 * frameWidth);
    if (m_hasButtons) {
        auto topButton = subControlRect(QStyle::SC_SpinBoxUp).size();
        auto bottomButton = subControlRect(QStyle::SC_SpinBoxDown).size();
        const auto height = topButton.height() + bottomButton.height();
        const auto buttonWidth = topButton.expandedTo(bottomButton).width();
        size.rwidth() += buttonWidth;
        size.setHeight(height);
    }

    return size;
}

QRectF SpinBoxElement::subControlRect(QStyle::SubControl subControl) const
{
    if (!m_isValid) {
        qCWarning(UNION_QTWIDGETS) << "subControlRect for " << subControl << "is not valid";
        return QRect();
    }

    QRectF rect;
    // Based on QCommonStyle. We only draw the "constrained" look for now.
    const QRectF buttonRect = m_layoutMap[ElementString::Indicator].rect;
    QRectF bgRect = m_spinBoxOption->rect;
    if (m_backgroundProperties->layout()) {
        bgRect.setWidth(qMax(bgRect.width(), m_backgroundProperties->layout()->width().value_or(0)));
        bgRect.setHeight(qMax(bgRect.height(), m_backgroundProperties->layout()->height().value_or(0)));
    }
    const bool noButtons = (m_spinBoxOption->buttonSymbols == QAbstractSpinBox::NoButtons);
    const int y = m_spinBoxOption->rect.y();
    const int x = m_spinBoxOption->rect.x() + m_spinBoxOption->rect.width() - buttonRect.width();

    if (subControl == QStyle::SC_SpinBoxUp) {
        rect = noButtons ? QRect() : QRect(x, y, buttonRect.width(), buttonRect.height());
    }
    if (subControl == QStyle::SC_SpinBoxDown) {
        rect = noButtons ? QRect() : QRect(x, y + buttonRect.height(), buttonRect.width(), buttonRect.height());
    }
    if (subControl == QStyle::SC_SpinBoxEditField) {
        if (noButtons) {
            rect = QRect(0, 0, bgRect.width(), bgRect.height());
        } else {
            rect = QRect(0, 0, x, bgRect.height());
        }
    }
    if (subControl == QStyle::SC_SpinBoxFrame) {
        rect = bgRect;
    }
    rect = m_style->visualRect(m_spinBoxOption->direction, m_spinBoxOption->rect, rect.toRect());
    return rect;
}

void SpinBoxElement::drawSpinIndicator(QPainter *painter, const QStyle::PrimitiveElement &primitive) const
{
    if (!m_isValid && !m_backgroundProperties && !m_backgroundProperties->icon()) {
        return;
    }

    QIcon indicatorIcon;
    switch (primitive) {
    case QStyle::PE_IndicatorSpinPlus:
        indicatorIcon = m_style->unionIcon(m_backgroundProperties, u"spinbox-increase"_s);
        break;
    case QStyle::PE_IndicatorSpinMinus:
        indicatorIcon = m_style->unionIcon(m_backgroundProperties, u"spinbox-decrease"_s);
        break;
    case QStyle::PE_IndicatorSpinUp:
        indicatorIcon = m_style->unionIcon(m_backgroundProperties, u"arrow-up-symbolic"_s);
        break;
    case QStyle::PE_IndicatorSpinDown:
        indicatorIcon = m_style->unionIcon(m_backgroundProperties, u"arrow-down-symbolic"_s);
        break;
    default:
        return;
    }
    drawIconAtRect(painter, indicatorIcon, m_styleOption->rect);
}

QStringList SpinBoxElement::elementHints() const
{
    QStringList hints;
    // Force the constrained look, as no other spinbox look will work due to QStyle expectations.
    hints.append(u"constrained"_s);
    return hints;
}

qreal SpinBoxElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_SpinBoxFrameWidth:
        return averageBorderSize();
    default:
        break;
    }
    return 0;
}
