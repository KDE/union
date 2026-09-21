// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ProgressBarElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include "elements/AbstractElement.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;
using namespace Union::Properties;

ProgressBarElement::ProgressBarElement(const QStyleOptionProgressBar *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_progressBarOption(option)
    , m_isHorizontal(true)
{
    update();
}

ProgressBarElement::~ProgressBarElement()
{
}

void ProgressBarElement::update()
{
    if (!m_progressBarOption) {
        return;
    }
    m_isHorizontal = m_progressBarOption->state.testFlag(QStyle::State_Horizontal);

    setText(m_progressBarOption->text);
    updateSubElementList();
    layout();
}

void ProgressBarElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_ProgressBar:
        drawBackground(painter);
        drawIndicator(painter);
        drawText(painter);
        break;
    case QStyle::CE_ProgressBarGroove:
        drawBackground(painter);
        break;
    case QStyle::CE_ProgressBarContents:
        drawIndicator(painter);
        break;
    case QStyle::CE_ProgressBarLabel:
        drawText(painter);
        break;
    }

    switch (enums.PrimitiveElement) {
    case QStyle::PE_IndicatorProgressChunk:
        drawChunk(painter);
        break;
    }
}

void ProgressBarElement::drawBackground(QPainter *painter) const
{
    auto groove = subElementRect(QStyle::SE_ProgressBarGroove);
    drawBackgroundRectangle(painter, groove, m_backgroundProperties);
}
void ProgressBarElement::drawIndicator(QPainter *painter) const
{
    auto progress = subElementRect(QStyle::SE_ProgressBarContents);
    drawBackgroundRectangle(painter, progress, m_contentProperties);
}

void ProgressBarElement::updateSubElementList()
{
    m_subElementList.clear();
    if (hasText()) {
        m_subElementList.append(ElementString::Text);
    }
}

void ProgressBarElement::layout()
{
    m_backgroundElementList = prepareElements(m_progressBarOption, m_widget);

    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
        m_layoutMap = layoutMap(m_backgroundElementList, m_progressBarOption, m_subElementList);
    }

    m_contentElementList = prepareElements(m_progressBarOption, m_widget, {ElementString::ProgressBar, ElementString::Track});

    if (!m_contentElementList.isEmpty()) {
        m_contentProperties = queryProperties(m_contentElementList);
        m_isValid = true;
    } else {
        m_isValid = false;
        qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
    }
}

QSizeF ProgressBarElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    // If the bar is vertical, make sure text has room
    auto size = applyPaddingToSize(contentsSizeFromStyle);
    if (!m_isHorizontal && hasText()) {
        size.rwidth() += m_layoutMap[ElementString::Text].rect.width();
    }
    return size;
}

QRectF ProgressBarElement::subElementRect(QStyle::SubElement element) const
{
    if (!m_progressBarOption) {
        return QRectF();
    }
    if (element == QStyle::SE_ProgressBarLabel) {
        // Copied and repurposed from Breeze
        const bool textVisible(m_progressBarOption->textVisible);
        const bool busy(m_progressBarOption->minimum == 0 && m_progressBarOption->maximum == 0);
        if (!textVisible || busy) {
            return QRect();
        }
        auto textFlags = textFlagsFromProperties(m_backgroundProperties);
        int textWidth = std::max(m_progressBarOption->fontMetrics.size(textFlags, m_progressBarOption->text).width(),
                                 m_progressBarOption->fontMetrics.size(textFlags, u"100%"_s).width());
        auto rect = centerRect(m_progressBarOption->rect, textWidth, m_progressBarOption->rect.height());
        rect.setLeft(rect.right() - textWidth + 1);
        rect = m_style->visualRect(m_progressBarOption->direction, m_progressBarOption->rect, rect.toRect());
        return rect;
    } else if (element == QStyle::SE_ProgressBarContents) {
        // Copied from Breeze
        const auto rect(subElementRect(QStyle::SE_ProgressBarGroove));
        const bool busy(m_progressBarOption->minimum == 0 && m_progressBarOption->maximum == 0);
        if (busy) {
            return rect;
        }
        bool reverse = (m_isHorizontal && (m_progressBarOption->direction == Qt::RightToLeft)) || !m_isHorizontal;
        if (m_progressBarOption->invertedAppearance) {
            reverse = !reverse;
        }
        const int progress(m_progressBarOption->progress - m_progressBarOption->minimum);
        const int steps(std::max(m_progressBarOption->maximum - m_progressBarOption->minimum, 1));
        const qreal position = qreal(progress) / qreal(steps);
        const int indicatorSize(position * (m_isHorizontal ? rect.width() : rect.height()));
        QRectF indicatorRect;
        if (m_isHorizontal) {
            indicatorRect = QRect(rect.left() + (reverse ? rect.width() - indicatorSize : 0), rect.y(), indicatorSize, rect.height());
        } else {
            indicatorRect = QRect(rect.x(), reverse ? (rect.bottom() - indicatorSize + 1) : rect.top(), rect.width(), indicatorSize);
        }
        return indicatorRect;
    } else if (element == QStyle::SE_ProgressBarGroove) {
        // Copied and repurposed from Breeze. We ignore width, we only want the thickness of the bar.
        const qreal styleHeight = m_backgroundProperties->safePropertyLookup(1.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::height);
        const qreal styleWidth = m_backgroundProperties->safePropertyLookup(1.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::width);
        const qreal height = m_progressBarOption->rect.height() <= 0 ? styleHeight : m_progressBarOption->rect.height();
        const qreal width = m_progressBarOption->rect.width() <= 0 ? styleWidth : m_progressBarOption->rect.width();

        auto rect = m_progressBarOption->rect;
        if (m_isHorizontal) {
            rect = centerRect(m_progressBarOption->rect, width, styleHeight).toRect();
        } else {
            rect = centerRect(m_progressBarOption->rect, styleWidth, height).toRect();
        }
        return m_style->visualRect(m_progressBarOption->direction, m_progressBarOption->rect, rect);
    };
    return QRectF();
}

void ProgressBarElement::drawChunk(QPainter *painter) const
{
    auto props = queryProperties(prepareElements(m_progressBarOption, m_widget, {ElementString::ProgressBar, ElementString::Chunk}));
    drawBackgroundRectangle(painter, m_progressBarOption->rect, props);
}

int ProgressBarElement::chunkWidth() const
{
    auto props = queryProperties(prepareElements(m_progressBarOption, m_widget, {ElementString::ProgressBar, ElementString::Chunk}));
    return props->safePropertyLookup(0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::width);
}

qreal ProgressBarElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ProgressBarChunkWidth:
        return chunkWidth();
    default:
        break;
    }
    return 0;
}

QStringList ProgressBarElement::elementHints() const
{
    QStringList hints;
    if (m_isHorizontal) {
        hints.append(u"horizontal"_s);
    } else {
        hints.append(u"vertical"_s);
    }
    return hints;
}
