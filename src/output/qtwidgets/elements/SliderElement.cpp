// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "SliderElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include "elements/AbstractElement.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;
using namespace Union::Properties;

SliderElement::SliderElement(const QStyleOptionSlider *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_sliderOption(option)
    , m_isHorizontal(false)
    , m_isInverted(false)
    , m_isReverse(false)
{
    update();
}

SliderElement::~SliderElement()
{
}

void SliderElement::update()
{
    if (!m_sliderOption) {
        return;
    }
    m_isHorizontal = m_sliderOption->state.testFlag(QStyle::State_Horizontal);
    m_isInverted = m_sliderOption->upsideDown;
    m_isReverse = m_isHorizontal && m_sliderOption->direction == Qt::RightToLeft;
    if (m_isInverted) {
        m_isReverse = !m_isReverse;
    }
    updateSubElementList();
    layout();
}

void SliderElement::layout()
{
    // Background is the groove
    m_backgroundElementList = prepareElements(m_sliderOption, m_widget);

    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
    }

    // Indicator is the handle
    m_indicatorElementList = prepareElements(m_sliderOption, m_widget, {ElementString::Handle});
    if (!m_indicatorElementList.isEmpty()) {
        m_indicatorProperties = queryProperties(m_indicatorElementList);
    }

    // Tickmarks
    m_tickElementList = prepareElements(m_sliderOption, m_widget, {ElementString::TickMark});
    if (!m_tickElementList.isEmpty()) {
        m_tickProperties = queryProperties(m_tickElementList);
    }

    // Contents is the fill
    m_contentElementList = prepareElements(m_sliderOption, m_widget, m_subElementList);
    if (!m_contentElementList.isEmpty()) {
        m_contentProperties = queryProperties(m_contentElementList);
        m_isValid = true;
    } else {
        m_isValid = false;
        qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
    }
}

void SliderElement::draw(QPainter *painter, DrawEnums enums) const
{
    Q_UNUSED(enums);
    if (!m_isValid || !m_sliderOption) {
        return;
    }

    // Background
    drawBackground(painter);

    // Progressbar
    const auto grooveRect = subControlRect(QStyle::SC_SliderGroove);
    const qreal p = m_sliderOption->sliderValue;
    const qreal min = m_sliderOption->minimum;
    const qreal max = m_sliderOption->maximum;
    const qreal percentage = (p - min) / (max - min);

    auto progress = grooveRect;

    if (m_isHorizontal) {
        const qreal progressWidth = percentage * grooveRect.width();
        if (m_isReverse) {
            progress.setLeft(grooveRect.right() - progressWidth);
        } else {
            progress.setWidth(progressWidth);
        }
    } else {
        const qreal progressHeight = percentage * grooveRect.height();
        if (m_isReverse) {
            progress.setTop(grooveRect.bottom() - progressHeight);
        } else {
            progress.setHeight(progressHeight);
        }
    }
    drawBackgroundRectangle(painter, progress, m_contentProperties);

    // Tickmark drawing is copied from breeze
    if (m_sliderOption && m_tickProperties && m_sliderOption->subControls.testFlag(QStyle::SC_SliderTickmarks)) {
        const auto &rect(m_sliderOption->rect);
        const int available(m_style->pixelMetric(QStyle::PM_SliderSpaceAvailable, m_styleOption, m_widget));
        int interval = m_sliderOption->tickInterval;
        if (interval < 1) {
            interval = m_sliderOption->pageStep;
        }
        if (interval >= 1) {
            // Offset the tickmarks to the center of the handle
            const int handleOffset(pixelMetric(QStyle::PM_SliderLength) / 2);
            int current(m_sliderOption->minimum);
            auto tickmarkElements = m_tickElementList;

            // store tick lines
            QList<QRectF> ticks = tickLines();
            // colors
            const auto reverse(m_sliderOption->direction == Qt::RightToLeft);
            while (current <= m_sliderOption->maximum) {
                // If tickmark is active, fetch the properties again with the active hint
                tickmarkElements.last()->setHint(u"active"_s, current <= m_sliderOption->sliderPosition);
                const auto props = queryProperties(tickmarkElements);

                // calculate positions and draw lines
                const int position(m_style->sliderPositionFromValue(m_sliderOption->minimum, m_sliderOption->maximum, current, available, m_isInverted)
                                   + handleOffset);
                for (const auto &tickLine : std::as_const(ticks)) {
                    if (m_isHorizontal) {
                        drawBackgroundRectangle(painter, tickLine.translated(reverse ? (rect.width() - position) : position, 0), props);
                    } else {
                        drawBackgroundRectangle(painter, tickLine.translated(0, position), props);
                    }
                }

                // go to next position
                current += interval;
            }
        }
    }

    // Handle
    auto handle = subControlRect(QStyle::SC_SliderHandle);
    drawBackgroundRectangle(painter, handle, m_indicatorProperties);
}

void SliderElement::drawBackground(QPainter *painter) const
{
    auto grooveRect = subControlRect(QStyle::SC_SliderGroove);
    drawBackgroundRectangle(painter, grooveRect, m_backgroundProperties);
}

void SliderElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(u"Fill"_s);
}

QSizeF SliderElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    if (!m_sliderOption) {
        return applyPaddingToSize(contentsSizeFromStyle);
    }

    // store tick position and orientation
    const QSlider::TickPosition tickPosition(m_sliderOption->tickPosition);
    const bool horizontal(m_sliderOption->orientation == Qt::Horizontal);
    const auto tick = tickMarkSize();

    // do nothing if no ticks are requested
    if (tickPosition == QSlider::NoTicks) {
        return applyPaddingToSize(contentsSizeFromStyle);
    }

    QSizeF size(contentsSizeFromStyle);
    if (horizontal) {
        if (tickPosition & QSlider::TicksAbove) {
            size.rheight() += tick.height();
        }
        if (tickPosition & QSlider::TicksBelow) {
            size.rheight() += tick.height();
        }
    } else {
        if (tickPosition & QSlider::TicksAbove) {
            size.rwidth() += tick.width();
        }
        if (tickPosition & QSlider::TicksBelow) {
            size.rwidth() += tick.width();
        }
    }

    return applyPaddingToSize(size);
}

QRectF SliderElement::subControlRect(QStyle::SubControl subControl) const
{
    if (!m_isValid || !m_sliderOption) {
        qCWarning(UNION_QTWIDGETS) << "subControlRect for " << subControl << "is not valid";
        return QRectF();
    }

    // Copied from Breeze
    auto rect(m_sliderOption->rect);
    auto frameWidth = m_style->pixelMetric(QStyle::PM_DefaultFrameWidth, m_sliderOption, m_widget);
    if (m_widget) {
        rect = m_widget->visibleRegion().boundingRect();
    }

    if (subControl == QStyle::SC_SliderHandle) {
        int handleHeight = 1;
        int handleWidth = 1;
        if (m_indicatorProperties->layout()) {
            handleHeight = m_indicatorProperties->layout()->height().value_or(6);
            handleWidth = m_indicatorProperties->layout()->width().value_or(6);
        }

        QRectF handleRect(centerRect(rect, handleWidth, handleHeight));
        const int sliderPos = m_style->sliderPositionFromValue(m_sliderOption->minimum,
                                                               m_sliderOption->maximum,
                                                               m_sliderOption->sliderPosition,
                                                               (m_isHorizontal ? (rect.width() - handleWidth) : (rect.height() - handleHeight)),
                                                               m_sliderOption->upsideDown);
        if (m_isHorizontal) {
            handleRect.moveLeft(rect.x() + sliderPos);
        } else {
            handleRect.moveTop(rect.y() + sliderPos);
        }
        handleRect = m_style->visualRect(m_sliderOption->direction, rect, handleRect);
        return handleRect;
    } else if (subControl == QStyle::SC_SliderGroove) {
        int grooveHeight = 1;
        int grooveWidth = 1;
        if (m_backgroundProperties->layout()) {
            grooveHeight = m_backgroundProperties->layout()->height().value_or(6);
            grooveWidth = m_backgroundProperties->layout()->width().value_or(6);
        }

        QRectF grooveRect = rect.adjusted(frameWidth, frameWidth, -frameWidth, -frameWidth);

        // centering
        if (m_isHorizontal) {
            grooveRect = centerRect(rect, grooveRect.width(), grooveHeight);
        } else {
            grooveRect = centerRect(rect, grooveWidth, grooveRect.height());
        }
        return m_style->visualRect(m_sliderOption->direction, rect, grooveRect);
    }
    return QRectF();
}

QList<QRectF> SliderElement::tickLines() const
{
    QList<QRectF> tickLines;
    if (!m_sliderOption) {
        return tickLines;
    }
    auto rect(m_sliderOption->rect);
    const auto grooveRect(subControlRect(QStyle::SC_SliderGroove));
    const int tickPosition(m_sliderOption->tickPosition);
    int interval = m_sliderOption->tickInterval;
    if (interval < 1) {
        interval = m_sliderOption->pageStep;
    }
    if (interval >= 1) {
        const auto tickSize = tickMarkSize();
        const QMarginsF tickMargins =
            safePropertyLookup(m_tickProperties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::margins, &SizePropertyGroup::toMargins);
        const auto tickMarginsWidth = tickMargins.left() + tickMargins.right();
        const auto tickMarginsHeight = tickMargins.top() + tickMargins.bottom();

        if (m_isHorizontal) {
            if (tickPosition & QSlider::TicksAbove) {
                tickLines.append(QRectF(rect.left() - 1, grooveRect.top() - tickMarginsHeight - tickSize.height(), tickSize.width(), tickSize.height()));
            }
            if (tickPosition & QSlider::TicksBelow) {
                tickLines.append(QRectF(rect.left() - 1, grooveRect.bottom() + tickMarginsHeight, tickSize.width(), tickSize.height()));
            }
        } else {
            if (tickPosition & QSlider::TicksAbove) {
                tickLines.append(QRectF(grooveRect.left() - tickMarginsWidth - tickSize.width(), rect.top() - 1, tickSize.width(), tickSize.height()));
            }
            if (tickPosition & QSlider::TicksBelow) {
                tickLines.append(QRectF(grooveRect.right() + tickMarginsWidth, rect.top() - 1, tickSize.width(), tickSize.height()));
            }
        }
    }
    return tickLines;
}

qreal SliderElement::controlThickness() const
{
    if (m_isValid && m_indicatorProperties) {
        QSizeF size = indicatorSize();
        QMarginsF padding =
            m_indicatorProperties->safePropertyLookup(QMarginsF(), &StylePropertyGroup::layout, &LayoutPropertyGroup::padding, &SizePropertyGroup::toMargins);
        size = size.shrunkBy(padding);
        if (m_sliderOption->orientation == Qt::Horizontal) {
            return size.height();
        } else {
            return size.width();
        }
    }
    return 0;
}

QStringList SliderElement::elementHints() const
{
    QStringList hints;
    if (!m_sliderOption) {
        return hints;
    }
    if (m_sliderOption->orientation == Qt::Horizontal) {
        hints.append(u"horizontal"_s);
    } else {
        hints.append(u"vertical"_s);
    }
    return hints;
}

qreal SliderElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_SliderLength:
    case QStyle::PM_SliderThickness:
    case QStyle::PM_SliderControlThickness:
        return controlThickness();
    default:
        break;
    }
    return 0;
}

QSizeF SliderElement::tickMarkSize() const
{
    if (!m_sliderOption) {
        return QSizeF();
    }
    const QSizeF tickSize{safePropertyLookup(m_tickProperties, 0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::width),
                          safePropertyLookup(m_tickProperties, 0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::height)};
    return tickSize;
}
