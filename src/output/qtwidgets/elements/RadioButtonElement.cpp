// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "RadioButtonElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

RadioButtonElement::RadioButtonElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
{
    setFlag(AbstractElementFlag::DrawMnemonics);
    update(option);
}

RadioButtonElement::~RadioButtonElement()
{
}

void RadioButtonElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    if (const auto newOpt = qstyleoption_cast<const QStyleOptionButton *>(option)) {
        m_buttonOption = newOpt;
    }
    if (m_buttonOption) {
        setIcon(m_buttonOption->icon);
        setText(m_buttonOption->text);
        updateSubElementList();
        updateGraphicsEffect(m_buttonOption->rect);
    }
    layout();
}

void RadioButtonElement::layout()
{
    m_indicatorElementList = prepareElements(m_styleOption, m_widget, {ElementString::RadioButton, ElementString::Indicator});
    if (!m_indicatorElementList.isEmpty()) {
        m_indicatorProperties = queryProperties(m_indicatorElementList);
    }
    m_elementList = prepareElements(m_styleOption, m_widget);
    if (!m_elementList.isEmpty()) {
        m_elementProperties = queryProperties(m_elementList);
        m_layoutMap = layoutMap(m_styleOption, m_subElementList);
        m_isValid = true;
    } else {
        m_isValid = false;
        qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
    }
}

void RadioButtonElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_RadioButton:
        drawBackground(painter);
        drawText(painter);
        drawIndicator(painter);
        break;
    case QStyle::CE_RadioButtonLabel:
        drawText(painter);
        break;
    }

    switch (enums.PrimitiveElement) {
    case QStyle::PE_IndicatorRadioButton:
        drawIndicator(painter);
        break;
    }
}

QSizeF RadioButtonElement::contentsSize(QSizeF contentsSizeFromStyle) const
{
    auto size = applyPaddingToSize(contentsSizeFromStyle);
    // Ensure indicator size is taken into account with the label
    size.rwidth() += spacing() + width(m_indicatorProperties);
    return size;
}

QRectF RadioButtonElement::subElementRect(QStyle::SubElement element) const
{
    if (m_isValid && m_buttonOption) {
        if (element == QStyle::SE_RadioButtonIndicator) {
            if (m_buttonOption->styleObject || m_widget) {
                // The indicator is drawn as part of something
                return m_layoutMap[ElementString::Indicator].rect;
            }
        }
    }
    // The indicator is drawn standalone (PE_IndicatorRadioButton for example)
    return m_style->QCommonStyle::subElementRect(element, m_styleOption, m_widget);
}

void RadioButtonElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(ElementString::Indicator);
    if (m_buttonOption) {
        if (!m_buttonOption->icon.isNull()) {
            m_subElementList.append(ElementString::Icon);
        }
        if (!m_buttonOption->text.isEmpty()) {
            m_subElementList.append(ElementString::Text);
        }
    }
}

void RadioButtonElement::drawIndicator(QPainter *painter) const
{
    UnionPainter unionPainter(painter, m_indicatorProperties);
    unionPainter.setRect(subElementRect(QStyle::SE_RadioButtonIndicator));
    unionPainter.draw();
}

Union::IdentifierList RadioButtonElement::elementHints() const
{
    Union::IdentifierList hints;
    if (m_buttonOption && !m_buttonOption->icon.isNull()) {
        hints.append(u"with-icon"_s);
    }
    return hints;
}

qreal RadioButtonElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ExclusiveIndicatorWidth:
        return width(m_indicatorProperties);
    case QStyle::PM_ExclusiveIndicatorHeight:
        return height(m_indicatorProperties);
    case QStyle::PM_RadioButtonLabelSpacing:
        return spacing();
    default:
        break;
    }
    return 0;
}

QString RadioButtonElement::fallbackElementName([[maybe_unused]] const QStyleOption *opt) const
{
    // QStyleOptionButton is also used by push buttons, so the option type would make this a Button
    return ElementString::RadioButton;
}
