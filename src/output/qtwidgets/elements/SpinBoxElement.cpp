// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "SpinBoxElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;
using namespace Union::Widgets;

SpinBoxElement::SpinBoxElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
    , m_hasButtons(true)
    , m_hasFrame(true)
    , m_usesArrows(true)
{
}

void SpinBoxElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    if (const auto newOpt = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
        m_spinBoxOption = newOpt;
    }
    if (m_spinBoxOption) {
        m_hasFrame = m_spinBoxOption->frame;
        m_hasButtons = (m_spinBoxOption->buttonSymbols != QAbstractSpinBox::NoButtons);
        m_usesArrows = (m_spinBoxOption->buttonSymbols == QAbstractSpinBox::UpDownArrows);
    }
    updateSubElementList();
    layout();
}

void SpinBoxElement::draw(QPainter *painter, DrawVariant what) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ComplexControl) {
    case QStyle::CC_SpinBox:
        if (m_hasFrame) {
            drawBackground(painter);
        } else {
            drawPanel(painter);
        }
        // For spinbox we need to manually create the indicator buttons
        if (m_hasButtons) {
            // Increase
            auto up = *option;
            up.rect = subControlRect(QStyle::SC_SpinBoxUp).toRect();
            style()->drawPrimitive(m_usesArrows ? QStyle::PE_IndicatorSpinUp : QStyle::PE_IndicatorSpinPlus, &up, painter);
            // Decrease
            auto down = *option;
            down.rect = subControlRect(QStyle::SC_SpinBoxDown).toRect();
            style()->drawPrimitive(m_usesArrows ? QStyle::PE_IndicatorSpinDown : QStyle::PE_IndicatorSpinMinus, &down, painter);
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

QSizeF SpinBoxElement::contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const
{
    const int frameWidth = style()->pixelMetric(QStyle::PM_SpinBoxFrameWidth, option, widget());
    auto size = contentsSizeFromStyle;
    size += QSize(2 * frameWidth, 2 * frameWidth);
    size.rwidth() += padding().left() + padding().right();
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

QRectF SpinBoxElement::subControlRect(const QStyleOption *option, QStyle::SubControl subControl) const
{
    if (m_isValid && m_spinBoxOption) {
        QRectF rect = option->rect;
        // Based on QCommonStyle. We only draw the "constrained" look for now.
        const QRectF buttonRect = m_layoutMap[ElementString::Indicator].rect;
        QRectF bgRect = m_spinBoxOption->rect;

        bgRect.setWidth(std::max(bgRect.width(), width()));
        bgRect.setHeight(std::max(bgRect.height(), height()));

        const bool noButtons = (m_spinBoxOption->buttonSymbols == QAbstractSpinBox::NoButtons);
        const int y = m_spinBoxOption->rect.y();
        const int x = m_spinBoxOption->rect.x() + m_spinBoxOption->rect.width() - buttonRect.width();

        if (subControl == QStyle::SC_SpinBoxUp) {
            rect = noButtons ? QRectF() : QRectF(x, y, buttonRect.width(), buttonRect.height());
        }
        if (subControl == QStyle::SC_SpinBoxDown) {
            rect = noButtons ? QRectF() : QRectF(x, y + buttonRect.height(), buttonRect.width(), buttonRect.height());
        }
        if (subControl == QStyle::SC_SpinBoxEditField) {
            rect = noButtons ? rect : rect.adjusted(0, 0, -buttonRect.width(), 0);
        }
        if (subControl == QStyle::SC_SpinBoxFrame) {
            rect = bgRect;
        }
        rect = style()->visualRect(m_spinBoxOption->direction, m_spinBoxOption->rect, rect);
        return rect;
    }
    return style()->QCommonStyle::subControlRect(QStyle::CC_SpinBox, qstyleoption_cast<const QStyleOptionSpinBox *>(option), subControl, widget());
}

void SpinBoxElement::drawSpinIndicator(QPainter *painter, QStyle::PrimitiveElement primitive) const
{
    if (!m_isValid || !option) {
        return;
    }

    QIcon indicatorIcon;
    switch (primitive) {
    case QStyle::PE_IndicatorSpinPlus:
        indicatorIcon = style()->unionIcon(m_elementProperties, u"spinbox-increase"_s);
        break;
    case QStyle::PE_IndicatorSpinMinus:
        indicatorIcon = style()->unionIcon(m_elementProperties, u"spinbox-decrease"_s);
        break;
    case QStyle::PE_IndicatorSpinUp:
        indicatorIcon = style()->unionIcon(m_elementProperties, u"arrow-up-symbolic"_s);
        break;
    case QStyle::PE_IndicatorSpinDown:
        indicatorIcon = style()->unionIcon(m_elementProperties, u"arrow-down-symbolic"_s);
        break;
    default:
        return;
    }
    drawIconAtRect(painter, indicatorIcon, option->rect);
}

Union::IdentifierList SpinBoxElement::elementHints() const
{
    Union::IdentifierList hints;
    // Force the constrained look, as no other spinbox look will work due to QStyle expectations.
    hints.append(u"constrained"_s);
    return hints;
}

std::optional<qreal> SpinBoxElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_SpinBoxFrameWidth:
        return averageBorderSize();
    default:
        break;
    }
    return std::nullopt;
}

QString SpinBoxElement::fallbackElementName([[maybe_unused]] const QStyleOption *opt) const
{
    return ElementString::SpinBox;
}
