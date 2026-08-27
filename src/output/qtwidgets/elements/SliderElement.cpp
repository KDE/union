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
    if (!m_isValid) {
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
    if (m_sliderOption->subControls.testFlag(QStyle::SC_SliderTickmarks)) {
        auto tickmarkElements = prepareElements(m_sliderOption, m_widget, {ElementString::TickMark});
        auto props = queryProperties(tickmarkElements);
        if (!props->layout()) {
            return;
        }
        auto rect = m_sliderOption->rect;
        const int available(m_style->pixelMetric(QStyle::PM_SliderSpaceAvailable, m_sliderOption, m_widget));
        int interval = m_sliderOption->tickInterval;

        if (interval < 1) {
            interval = m_sliderOption->pageStep;
        }
        if (interval >= 1) {
            const int fudge(m_style->pixelMetric(QStyle::PM_SliderLength, m_sliderOption, m_widget) / 2);
            int current(m_sliderOption->minimum);

            auto ticks = tickLines();

            // colors
            const auto reverse(m_sliderOption->direction == Qt::RightToLeft);
            while (current <= m_sliderOption->maximum) {
                // adjust color
                tickmarkElements.last()->setHint(u"active"_s, current <= m_sliderOption->sliderPosition);
                auto props = queryProperties(tickmarkElements);
                const auto color = props->background()->color()->toQColor();
                painter->setPen(color);

                // calculate positions and draw lines
                const int position(m_style->sliderPositionFromValue(m_sliderOption->minimum, m_sliderOption->maximum, current, available, m_isInverted)
                                   + fudge);
                for (const QRect &tickLine : std::as_const(ticks)) {
                    if (m_isHorizontal) {
                        painter->drawRect(tickLine.translated(reverse ? (rect.width() - position) : position, 0));
                    } else {
                        painter->drawRect(tickLine.translated(0, position));
                    }
                }
                // go to next position
                current += interval;
            }
        }
    }

    // Handle
    // TODO: the handle doesnt center on the tickmark yet.
    // This requires making the contentSize large enough to hold the extents of the
    // handle, and then drawing it in correct position.
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
    auto grooveRect = subControlRect(QStyle::SC_SliderGroove).size();
    auto tickRect = subControlRect(QStyle::SC_SliderTickmarks).size();
    auto handleRect = subControlRect(QStyle::SC_SliderHandle).size();
    auto combinedSize = grooveRect.expandedTo(tickRect.expandedTo(handleRect));
    // Add some extra spacing so that we can draw the handle outside of the groove
    if (m_isHorizontal) {
        combinedSize.rwidth() += handleRect.width();
    } else {
        combinedSize.rheight() += handleRect.height();
    }
    return contentsSizeFromStyle.expandedTo(applyPaddingToSize(combinedSize));
}

QRectF SliderElement::subControlRect(QStyle::SubControl subControl) const
{
    if (!m_isValid) {
        qCWarning(UNION_QTWIDGETS) << "subControlRect for " << subControl << "is not valid";
        return QRect();
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
        handleRect = m_style->visualRect(m_sliderOption->direction, rect, handleRect.toRect());
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
        return m_style->visualRect(m_sliderOption->direction, rect, grooveRect.toRect());
    } else if (subControl == QStyle::SC_SliderTickmarks && m_sliderOption->tickPosition != QSlider::NoTicks) {
        QRegion r;
        r.setRects(tickLines());
        return r.boundingRect();
    }
    return QRect();
}

QList<QRect> SliderElement::tickLines() const
{
    QList<QRect> tickLines;
    auto tickMarkProps = queryProperties(prepareElements(m_sliderOption, m_widget, {ElementString::TickMark}));
    if (!tickMarkProps && !tickMarkProps->layout()) {
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
        const QSizeF tickSize(tickMarkProps->layout()->width().value_or(0), tickMarkProps->layout()->height().value_or(0));
        const QMarginsF tickMargins = tickMarkProps->layout()->margins()->toMargins().toMargins();
        const auto tickMarginsWidth = tickMargins.left() + tickMargins.right();
        const auto tickMarginsHeight = tickMargins.top() + tickMargins.bottom();

        // store tick lines
        if (m_isHorizontal) {
            if (tickPosition & QSlider::TicksAbove) {
                tickLines.append(QRect(rect.left(),
                                       grooveRect.top() - tickMarginsHeight,
                                       rect.left(),
                                       grooveRect.top() - tickMarginsHeight - tickSize.height() - grooveRect.height()));
            }
            if (tickPosition & QSlider::TicksBelow) {
                tickLines.append(
                    QRect(rect.left(), grooveRect.bottom() + tickMarginsHeight, rect.left(), grooveRect.bottom() + tickMarginsHeight + tickSize.height()));
            }

        } else {
            if (tickPosition & QSlider::TicksAbove) {
                tickLines.append(QRect(grooveRect.left() - tickMarginsWidth,
                                       rect.top(),
                                       grooveRect.left() - tickMarginsWidth - tickSize.width() - grooveRect.width(),
                                       rect.top()));
            }
            if (tickPosition & QSlider::TicksBelow) {
                tickLines.append(
                    QRect(grooveRect.right() + tickMarginsWidth, rect.top(), grooveRect.right() + tickMarginsWidth + tickSize.width(), rect.top()));
            }
        }
    }
    return tickLines;
}

qreal SliderElement::controlThickness() const
{
    if (m_isValid && m_indicatorProperties && m_indicatorProperties->layout()) {
        QSizeF size(m_indicatorProperties->layout()->width().value_or(1), m_indicatorProperties->layout()->height().value_or(1));
        if (m_indicatorProperties->layout()->padding()) {
            size = size.shrunkBy(m_indicatorProperties->layout()->padding()->toMargins().toMargins());
        }
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
        return height();
    case QStyle::PM_SliderThickness:
    case QStyle::PM_SliderControlThickness:
        return controlThickness();
    default:
        break;
    }
    return 0;
}
