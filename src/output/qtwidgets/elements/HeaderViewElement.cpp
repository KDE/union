// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "HeaderViewElement.h"

#include <QHeaderView>

#include "IndicatorSubElement.h"
#include "TextSubElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;
using namespace Union::Widgets;

static OptionToElementRegistration<HeaderViewElement, QStyleOption::SO_Header> headerOptionRegistration;
static WidgetToElementRegistration<HeaderViewElement, QHeaderView> headerWidgetRegistration;

HeaderViewElement::HeaderViewElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
    , m_isHorizontal(false)
{
}

void HeaderViewElement::update(const QStyleOption *option)
{
    auto e = element();
    if (!e) {
        e = Union::Element::create();
        e->setType(ElementString::HeaderViewDelegate);
        setElement(e);
    }

    auto headerOption = qstyleoption_cast<const QStyleOptionHeader *>(option);
    if (!headerOption) {
        return AbstractElement::update(option);
    }

    m_isHorizontal = (headerOption->orientation == Qt::Horizontal);
    e->setHint(u"horizontal"_s, m_isHorizontal);
    e->setHint(u"vertical"_s, !m_isHorizontal);

    // Due to how Breeze style does this, these need to be inverted
    // to follow QtQuick style
    e->setHint(u"sort-ascending"_s, headerOption->sortIndicator == QStyleOptionHeader::SortDown);
    e->setHint(u"sort-descending"_s, headerOption->sortIndicator == QStyleOptionHeader::SortUp);

    withFirstChild<TextSubElement>(headerOption->text.isEmpty(), [&](auto element) {
        element->setText(headerOption->text);
    });

    // if (!headerOption->text.isEmpty()) {
    //     firstChildOfType<TextSubElement>(true)->setText(headerOption->text);
    // } else {
    //     removeChildOfType<TextSubElement>();
    // }

    // if (const auto newOpt = ) {
    //     m_headerOption = newOpt;
    // }
    // if (m_headerOption) {
    //     setText(m_headerOption->text);
    //     setIcon(m_headerOption->icon);
    //     updateSubElementList();
    // }
    // layout();
    // setIndicator(sortIndicator());

    AbstractElement::update(option);
}

// QIcon HeaderViewElement::sortIndicator()
// {
//     QIcon sortIndicator;
//     if (m_elementProperties && m_isValid && m_headerOption) {
//         switch (m_headerOption->sortIndicator) {
//         case QStyleOptionHeader::None:
//             break;
//         case QStyleOptionHeader::SortUp:
//             sortIndicator = style()->unionIcon(m_elementProperties, u"arrow-down-symbolic"_s);
//             break;
//         case QStyleOptionHeader::SortDown:
//             sortIndicator = style()->unionIcon(m_elementProperties, u"arrow-up-symbolic"_s);
//             break;
//         }
//     }
//     return sortIndicator;
// }

// void HeaderViewElement::layout()
// {
//     m_elementList = prepareElements(option, widget(), {ElementString::HeaderViewDelegate});
//     if (!m_elementList.isEmpty()) {
//         m_elementProperties = queryProperties(m_elementList);
//         m_layoutMap = layoutMap(option, m_subElementList);
//         m_isValid = true;
//     } else {
//         m_isValid = false;
//         qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
//     }
// }

void HeaderViewElement::draw(QPainter *painter, DrawVariant what) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_Header:
        drawBackground(painter);
        drawIcon(painter);
        drawText(painter);
        drawIndicator(painter);
        break;
    case QStyle::CE_HeaderSection:
        drawBackground(painter);
        break;
    case QStyle::CE_HeaderLabel:
        // Headerlabel can be drawn anywhere in custom controls,
        // so rely on default behavior for them.
        style()->QCommonStyle::drawControl(QStyle::CE_HeaderLabel, option, painter, widget());
        break;
    case QStyle::CE_HeaderEmptyArea:
        drawBackground(painter);
        break;
    }

    switch (enums.PrimitiveElement) {
    case QStyle::PE_IndicatorHeaderArrow:
        drawIndicator(painter);
        break;
    }
}

// void HeaderViewElement::drawText(QPainter *painter) const
// {
//     if (!m_text.isEmpty() && isValid()) {
//         QRectF textRect = subElementRect(QStyle::SE_HeaderLabel);
//         drawTextAtRect(painter, m_text, textRect);
//     }
// }

// void HeaderViewElement::updateSubElementList()
// {
//     m_subElementList.clear();
//     if (hasIcon()) {
//         m_subElementList.append(ElementString::Icon);
//     }
//     if (hasText()) {
//         m_subElementList.append(ElementString::Text);
//     }
//     if (m_headerOption && m_headerOption->sortIndicator != QStyleOptionHeader::None) {
//         m_subElementList.append(ElementString::Indicator);
//     }
// }

std::optional<QRectF> HeaderViewElement::subElementRect(const QStyleOption *option, QStyle::SubElement element) const
{
    // if (m_isValid) {
    QRectF r;
    switch (element) {
    case QStyle::SE_HeaderLabel:
        // rect = m_layoutMap[ElementString::Text].rect;
        break;
    case QStyle::SE_HeaderArrow:
        // rect = m_layoutMap[ElementString::Indicator].rect;
        break;
    default:
        break;
    }

    auto boundingRect = rect();
    auto padding = propertyWrapper().padding();

    // Ensure we do not draw outside of the rectangle
    if (r.x() < boundingRect.x() + padding.left()) {
        r.setX(boundingRect.x() + padding.left());
    }

    if (r.width() > boundingRect.width() - padding.right()) {
        r.setWidth(boundingRect.width() - padding.right());
    }

    if (r.isValid()) {
        return UnionStyle::visualRect(direction(), rect(), r);
    } else {
        return std::nullopt;
    }
}

QSizeF HeaderViewElement::contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const
{
    if (!m_isValid || !m_headerOption) {
        return contentsSizeFromStyle;
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
        contentsHeight = std::max(contentsHeight, iconSize().height());
    }

    if (m_isHorizontal && m_headerOption->sortIndicator != QStyleOptionHeader::None) {
        // also add space for sort indicator
        contentsWidth += arrowWidth + space;
        contentsHeight = std::max(contentsHeight, arrowWidth);
    }

    // update contents size, add margins and return
    const QSizeF size(contentsSizeFromStyle.expandedTo(QSize(contentsWidth, contentsHeight)));
    return applyPaddingToSize(size);
}

// Union::IdentifierList HeaderViewElement::elementHints() const
// {
//     Union::IdentifierList hints;
//     if (m_headerOption) {
//         switch (m_headerOption->sortIndicator) {
//         case QStyleOptionHeader::None:
//             return hints;
//         // Due to how Breeze style does this, these need to be inverted
//         // to follow QtQuick style
//         case QStyleOptionHeader::SortUp:
//             hints.append(u"sort-descending"_s);
//             break;
//         case QStyleOptionHeader::SortDown:
//             hints.append(u"sort-ascending"_s);
//             break;
//         }
//     }
//     return hints;
// }

std::optional<qreal> HeaderViewElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_HeaderMargin:
    case QStyle::PM_HeaderGripMargin:
        return propertyWrapper().averagePadding();
    case QStyle::PM_HeaderMarkSize:
        return propertyWrapper().iconSize().width();
    default:
        break;
    }
    return std::nullopt;
}

// QString HeaderViewElement::fallbackElementName([[maybe_unused]] const QStyleOption *opt) const
// {
//     return ElementString::HeaderViewDelegate;
// }
