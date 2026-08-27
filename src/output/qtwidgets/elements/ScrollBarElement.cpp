// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ScrollBarElement.h"
#include "SharedNames.h"
#include "StyleUtils.h"
#include "UnionStyle.h"
#include "elements/AbstractElement.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

ScrollBarElement::ScrollBarElement(const QStyleOptionSlider *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_scrollBarOption(option)
    , m_horizontal(false)
{
    update();
}

ScrollBarElement::~ScrollBarElement()
{
}

void ScrollBarElement::update()
{
    m_horizontal = (m_scrollBarOption->state.testFlag(QStyle::State_Horizontal));
    updateSubElementList();
    layout();
}

void ScrollBarElement::layout()
{
    // Background and content is separate
    m_backgroundElementList = prepareElements(m_scrollBarOption, m_widget);
    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
        m_layoutMap = layoutMap(m_backgroundElementList, m_scrollBarOption, m_subElementList);
    }

    m_indicatorElementList = prepareElements(m_scrollBarOption, m_widget, {ElementString::Handle});
    if (!m_indicatorElementList.empty()) {
        m_indicatorProperties = queryProperties(m_indicatorElementList);
    }

    m_contentElementList = prepareElements(m_scrollBarOption, m_widget, m_subElementList);
    if (!m_contentElementList.isEmpty()) {
        m_contentProperties = queryProperties(m_contentElementList);
        m_isValid = true;
    } else {
        m_isValid = false;
        qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
    }
}

void ScrollBarElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_ScrollBarSlider:
        drawIndicator(painter);
        break;
    case QStyle::CE_ScrollBarAddLine:
    case QStyle::CE_ScrollBarSubLine:
    case QStyle::CE_ScrollBarAddPage:
    case QStyle::CE_ScrollBarSubPage:
    case QStyle::CE_ScrollBarFirst:
    case QStyle::CE_ScrollBarLast:
        break;
    }

    switch (enums.ComplexControl) {
    case QStyle::CC_ScrollBar:
        drawBackground(painter);
        drawIndicator(painter);
        break;
    }
}

void ScrollBarElement::drawBackground(QPainter *painter) const
{
    const auto rect = subControlRect(QStyle::SC_ScrollBarGroove);
    drawBackgroundRectangle(painter, rect, m_backgroundProperties);
}

void ScrollBarElement::drawIndicator(QPainter *painter) const
{
    if (m_scrollBarOption->subControls & QStyle::SC_ScrollBarSlider) {
        QStyleOptionSlider subopt = *m_scrollBarOption;
        subopt.rect = m_scrollBarOption->rect;
        subopt.state = m_scrollBarOption->state;
        subopt.rect = subControlRect(QStyle::SC_ScrollBarSlider).toRect();
        if (subopt.rect.isValid()) {
            if (!(m_scrollBarOption->activeSubControls & QStyle::SC_ScrollBarSlider)) {
                subopt.state &= ~(QStyle::State_Sunken | QStyle::State_MouseOver);
            }
            drawBackgroundRectangle(painter, subopt.rect, m_indicatorProperties);

            if (m_scrollBarOption->state & QStyle::State_HasFocus) {
                m_style->drawPrimitive(QStyle::PE_FrameFocusRect, &subopt, painter);
            }
        }
    }
}

void ScrollBarElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(ElementString::Handle);
}

QSizeF ScrollBarElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    return contentsSizeFromStyle;
}

QRectF ScrollBarElement::subControlRect(QStyle::SubControl subControl) const
{
    if (!m_isValid) {
        qCWarning(UNION_QTWIDGETS) << "subControlRect for " << subControl << "is not valid";
        return QRect();
    }

    // Copied from Breeze
    auto rect = m_scrollBarOption->rect;
    if (m_widget) {
        rect = m_widget->visibleRegion().boundingRect();
    }
    if (subControl == QStyle::SC_ScrollBarSlider) {
        auto groove = m_style->visualRect(m_scrollBarOption->direction, rect, subControlRect(QStyle::SC_ScrollBarGroove).toRect());

        int space(m_horizontal ? groove.width() : groove.height());
        int thickness = 0;
        QMarginsF padding;

        if (m_backgroundProperties->layout() && m_backgroundProperties->layout()->padding()) {
            padding = m_backgroundProperties->layout()->padding()->toMargins().toMargins();
            if (m_scrollBarOption->orientation == Qt::Horizontal) {
                thickness = m_backgroundProperties->layout()->height().value_or(0);
            } else {
                thickness = m_backgroundProperties->layout()->width().value_or(0);
            }
        }

        // Return early with just padding changes
        if (m_scrollBarOption->minimum == m_scrollBarOption->maximum) {
            const auto rect = QRect(groove.left(), groove.top(), groove.width(), groove.height());
            if (m_horizontal) {
                return m_style->visualRect(m_scrollBarOption->direction, rect, rect.marginsRemoved(padding.toMargins()));
            } else {
                return m_style->visualRect(m_scrollBarOption->direction, rect, rect.marginsRemoved(padding.toMargins()));
            }
        }

        int sliderSize = space * qreal(m_scrollBarOption->pageStep) / (m_scrollBarOption->maximum - m_scrollBarOption->minimum + m_scrollBarOption->pageStep);
        sliderSize = qMax(sliderSize, qMax(thickness, m_style->pixelMetric(QStyle::PM_ScrollBarSliderMin, m_scrollBarOption, m_widget)));
        sliderSize = qMin(sliderSize, space);
        space -= sliderSize;
        if (space <= 0) {
            const auto rect = QRect(groove.left(), groove.top(), groove.width(), groove.height());
            if (m_horizontal) {
                return m_style->visualRect(m_scrollBarOption->direction, rect, rect.marginsRemoved(padding.toMargins()));
            } else {
                return m_style->visualRect(m_scrollBarOption->direction, rect, rect.marginsRemoved(padding.toMargins()));
            }
        }
        int pos =
            qRound(qreal(m_scrollBarOption->sliderPosition - m_scrollBarOption->minimum) / (m_scrollBarOption->maximum - m_scrollBarOption->minimum) * space);
        if (m_scrollBarOption->upsideDown) {
            pos = space - pos;
        }
        if (m_horizontal) {
            const auto rect = QRect(groove.left() + pos, groove.top(), sliderSize, groove.height());
            return m_style->visualRect(m_scrollBarOption->direction, rect, rect.marginsRemoved(padding.toMargins()));
        } else {
            const auto rect = QRect(groove.left(), groove.top() + pos, groove.width(), sliderSize);
            return m_style->visualRect(m_scrollBarOption->direction, rect, rect.marginsRemoved(padding.toMargins()));
        }
    } else if (subControl == QStyle::SC_ScrollBarGroove) {
        return m_style->visualRect(m_scrollBarOption->direction, m_scrollBarOption->rect, rect);
    } else {
        return QRect();
    }
}

qreal ScrollBarElement::extent() const
{
    if (m_isValid && m_backgroundProperties && m_backgroundProperties->layout()) {
        if (m_scrollBarOption->orientation == Qt::Horizontal) {
            return m_backgroundProperties->layout()->height().value_or(0);
        } else {
            return m_backgroundProperties->layout()->width().value_or(0);
        }
    }
    return 0;
}

qreal ScrollBarElement::controlThickness() const
{
    if (m_isValid && m_indicatorProperties && m_indicatorProperties->layout()) {
        QSizeF size(m_indicatorProperties->layout()->width().value_or(1), m_indicatorProperties->layout()->height().value_or(1));
        if (m_indicatorProperties->layout()->padding()) {
            size = size.shrunkBy(m_indicatorProperties->layout()->padding()->toMargins().toMargins());
        }
        if (m_scrollBarOption->orientation == Qt::Horizontal) {
            return size.height();
        } else {
            return size.width();
        }
    }
    return 0;
}

qreal ScrollBarElement::minimumSize() const
{
    return extent() * 2;
}

QStringList ScrollBarElement::elementHints() const
{
    QStringList hints;
    if (m_scrollBarOption->orientation == Qt::Horizontal) {
        hints.append(u"horizontal"_s);
    } else {
        hints.append(u"vertical"_s);
    }
    return hints;
}

qreal ScrollBarElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ScrollBarExtent:
        return extent();
    case QStyle::PM_SliderControlThickness:
        return controlThickness();
    case QStyle::PM_ScrollBarSliderMin:
        return minimumSize();
    default:
        break;
    }
    return 0;
}
