// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ProgressBarElement.h"

#include <QProgressBar>

#include "TextSubElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;
using namespace Union::Widgets;

static OptionToElementRegistration<ProgressBarElement, QStyleOption::SO_ProgressBar> progressOptionRegistration;
static WidgetToElementRegistration<ProgressBarElement, QProgressBar> progressWidgetRegistration;

ProgressBarElement::ProgressBarElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
    m_filledIndicator = new IndicatorSubElement(style, widget, ElementString::Filled);
    addChildElement(m_filledIndicator);
}

void ProgressBarElement::update(const QStyleOption *option)
{
    auto e = ensureElement();
    e->setType(ElementString::ProgressBar);

    const auto progressOption = qstyleoption_cast<const QStyleOptionProgressBar *>(option);
    if (!progressOption) {
        return AbstractElement::update(option);
    }

    m_isHorizontal = progressOption->state.testFlag(QStyle::State_Horizontal);
    e->setHint(u"horizontal"_s, m_isHorizontal);
    e->setHint(u"vertical"_s, !m_isHorizontal);

    withFirstChild<TextSubElement>(!progressOption->text.isEmpty() && progressOption->textVisible, [&](auto element) {
        element->setText(progressOption->text);
    });

    AbstractElement::update(option);

    m_filledIndicator->setRect(filledIndicatorRect(progressOption));
}

void ProgressBarElement::draw(QPainter *painter, DrawVariant what) const
{
    std::visit(overloaded{//
                          [](std::nullopt_t) { },
                          [](QStyle::ComplexControl) { },
                          [&](QStyle::ControlElement controlElement) {
                              switch (controlElement) {
                              case QStyle::CE_ProgressBar:
                                  drawBackground(painter);
                                  drawChildren(painter);
                                  break;
                              case QStyle::CE_ProgressBarGroove:
                                  drawBackground(painter);
                                  break;
                              case QStyle::CE_ProgressBarContents:
                                  drawChildren<IndicatorSubElement>(painter);
                                  // drawIndicator(painter);
                                  break;
                              case QStyle::CE_ProgressBarLabel:
                                  drawChildren<TextSubElement>(painter);
                                  break;
                              default:
                                  break;
                              }
                          },
                          [&](QStyle::PrimitiveElement primitive) {
                              if (primitive == QStyle::PE_IndicatorProgressChunk) {
                                  drawChildren<IndicatorSubElement>(painter);
                              }
                          }},
               what);
}

QSizeF ProgressBarElement::contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const
{
    // If the bar is vertical, make sure text has room
    auto size = propertyWrapper().adjustSize(contentsSizeFromStyle, false);
    if (auto text = firstChildOfType<TextSubElement>()) {
        size.rwidth() = std::max(size.width(), text->rect().width());
        size.rheight() = std::max(size.height(), text->rect().height());
    }
    return size;
}

std::optional<QRectF> ProgressBarElement::subElementRect([[maybe_unused]] const QStyleOption *option, QStyle::SubElement element) const
{
    switch (element) {
    case QStyle::SE_ProgressBarLabel: {
        auto text = firstChildOfType<TextSubElement>();
        if (text) {
            return text->rect();
        }
        break;
    }
    case QStyle::SE_ProgressBarContents:
        return m_filledIndicator->rect();
    case QStyle::SE_ProgressBarGroove: {
        // Copied and repurposed from Breeze. We ignore width, we only want the thickness of the bar.
        auto r = rect();
        const qreal styleWidth = propertyWrapper().width();
        const qreal styleHeight = propertyWrapper().height();
        const qreal width = r.width() <= 0 ? styleWidth : r.width();
        const qreal height = r.height() <= 0 ? styleHeight : r.height();

        if (m_isHorizontal) {
            r = centerRect(r, width, styleHeight);
        } else {
            r = centerRect(r, styleWidth, height);
        }
        return UnionStyle::visualRect(direction(), rect(), r);
    }
    default:
        break;
    }

    return std::nullopt;
}

std::optional<qreal> ProgressBarElement::pixelMetric([[maybe_unused]] const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ProgressBarChunkWidth:
        return m_filledIndicator->propertyWrapper().width();
    default:
        break;
    }
    return std::nullopt;
}

void ProgressBarElement::drawBackground(QPainter *painter, UnionPainter::Parts parts) const
{
    UnionPainter unionPainter(painter, properties());
    unionPainter.setRect(subElementRect(nullptr, QStyle::SE_ProgressBarGroove).value_or(rect()));
    unionPainter.setParts(parts);
    unionPainter.draw();
}

QRectF ProgressBarElement::filledIndicatorRect(const QStyleOptionProgressBar *progressOption)
{
    // Copied from Breeze
    const auto rect = subElementRect(progressOption, QStyle::SE_ProgressBarGroove).value_or(QRectF{});
    const bool busy(progressOption->minimum == 0 && progressOption->maximum == 0);
    if (busy) {
        return rect;
    }
    bool reverse = (m_isHorizontal && (progressOption->direction == Qt::RightToLeft)) || !m_isHorizontal;
    if (progressOption->invertedAppearance) {
        reverse = !reverse;
    }
    const int progress(progressOption->progress - progressOption->minimum);
    if (progress < 0) {
        return rect;
    }
    const int steps(std::max(progressOption->maximum - progressOption->minimum, 1));
    const qreal position = qreal(progress) / qreal(steps);
    const qreal indicatorSize(position * (m_isHorizontal ? rect.width() : rect.height()));
    QRectF indicatorRect;
    if (m_isHorizontal) {
        indicatorRect = {rect.left() + (reverse ? rect.width() - indicatorSize : 0), rect.y(), indicatorSize, rect.height()};
    } else {
        indicatorRect = {rect.x(), reverse ? (rect.bottom() - indicatorSize + 1) : rect.top(), rect.width(), indicatorSize};
    }
    return indicatorRect;
}
