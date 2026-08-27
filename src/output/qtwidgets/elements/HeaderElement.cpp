// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "HeaderElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

HeaderElement::HeaderElement(const QStyleOptionHeader *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_headerOption(option)
    , m_isHorizontal(false)
{
    update();
}

HeaderElement::~HeaderElement()
{
}

void HeaderElement::update()
{
    if (!m_headerOption) {
        m_isValid = false;
        return;
    }
    m_isHorizontal = (m_headerOption->orientation == Qt::Horizontal);
    setText(m_headerOption->text);
    updateSubElementList();
    layout();
}

QIcon HeaderElement::sortIndicator()
{
    QIcon sortIndicator;
    if (m_contentProperties) {
        switch (m_headerOption->sortIndicator) {
        case QStyleOptionHeader::None:
            break;
        case QStyleOptionHeader::SortUp:
            if (m_contentProperties->icon()) {
                sortIndicator = m_style->unionIcon(m_contentProperties, u"arrow-up-symbolic"_s);
            }
            break;
        case QStyleOptionHeader::SortDown:
            if (m_contentProperties->icon()) {
                sortIndicator = m_style->unionIcon(m_contentProperties, u"arrow-down-symbolic"_s);
            }
            break;
        }
    }
    return sortIndicator;
}

void HeaderElement::layout()
{
    // Background and content is separate

    m_backgroundElementList = prepareElements(m_headerOption, m_widget, {ElementString::HeaderViewDelegate});
    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
        m_layoutMap = layoutMap(m_backgroundElementList, m_headerOption, m_subElementList);
    }

    m_contentElementList = prepareElements(m_headerOption, m_widget, {ElementString::HeaderViewDelegate});
    if (!m_contentElementList.isEmpty()) {
        m_contentProperties = queryProperties(m_contentElementList);
        setIcon(sortIndicator());
        m_isValid = true;
    } else {
        m_isValid = false;
        qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
    }
}

void HeaderElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_Header:
        drawBackground(painter);
        drawIcon(painter);
        drawText(painter);
        break;
    case QStyle::CE_HeaderSection:
        drawBackground(painter);
        break;
    case QStyle::CE_HeaderLabel:
        drawText(painter);
        break;
    case QStyle::CE_HeaderEmptyArea:
        break;
    }

    switch (enums.PrimitiveElement) {
    case QStyle::PE_IndicatorHeaderArrow:
        drawIcon(painter);
        break;
    }
}

void HeaderElement::updateSubElementList()
{
    m_subElementList = {ElementString::Text, ElementString::Icon};
}

QRectF HeaderElement::subElementRect(QStyle::SubElement element) const
{
    if (!m_isValid) {
        qCWarning(UNION_QTWIDGETS) << "Subelementrect for " << element << "is not valid";
        return QRect();
    }
    QRectF rect;
    if (element == QStyle::SE_HeaderArrow || element == QStyle::SE_HeaderLabel) {
        auto mapItem = (element == QStyle::SE_HeaderLabel) ? ElementString::Text : ElementString::Icon;
        rect = m_layoutMap[mapItem].rect;
    }
    return rect;
}

QSizeF HeaderElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    if (!m_isValid) {
        return QSizeF();
    }
    // Copied from Breeze
    const qreal arrowWidth = subElementRect(QStyle::SE_HeaderArrow).width();
    const qreal space = spacing();

    const QSizeF textSize(hasText() ? m_headerOption->fontMetrics.size(0, m_headerOption->text) : QSize());

    // contents width
    qreal contentsWidth(0);
    if (hasText()) {
        contentsWidth += textSize.width();
    }
    if (hasIcon()) {
        contentsWidth += iconSize().width();
        if (hasText()) {
            contentsWidth += space;
        }
    }

    // contents height
    qreal contentsHeight(hasText() ? textSize.height() : m_headerOption->fontMetrics.height());
    if (hasIcon()) {
        contentsHeight = qMax(contentsHeight, iconSize().height());
    }

    if (m_isHorizontal && m_headerOption->sortIndicator != QStyleOptionHeader::None) {
        // also add space for sort indicator
        contentsWidth += arrowWidth + space;
        contentsHeight = qMax(contentsHeight, arrowWidth);
    }

    // update contents size, add margins and return
    const QSizeF size(contentsSizeFromStyle.expandedTo(QSize(contentsWidth, contentsHeight)));
    return applyPaddingToSize(size);
}

QStringList HeaderElement::elementHints() const
{
    QStringList hints;
    switch (m_headerOption->sortIndicator) {
    case QStyleOptionHeader::None:
        return hints;
    case QStyleOptionHeader::SortUp:
        hints.append(u"sort-ascending"_s);
        break;
    case QStyleOptionHeader::SortDown:
        hints.append(u"sort-descending"_s);
        break;
    }
    return hints;
}

qreal HeaderElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_HeaderMargin:
        return spacing();
    case QStyle::PM_HeaderDefaultSectionSizeHorizontal:
        return width();
    case QStyle::PM_HeaderDefaultSectionSizeVertical:
        return height();
    case QStyle::PM_HeaderMarkSize:
        return indicatorSize().width();
    default:
        break;
    }
    return 0;
}
