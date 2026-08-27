// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ComboBoxElement.h"
#include "SharedNames.h"
#include "StyleUtils.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

ComboBoxElement::ComboBoxElement(const QStyleOptionComboBox *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_comboBoxOption(option)
    , m_spacing(0)
    , m_editable(false)
{
    update();
}

void ComboBoxElement::update()
{
    m_editable = m_comboBoxOption->editable;

    setIndicator(QIcon());
    m_spacing = 0;
    m_indicatorElementList = prepareElements(m_comboBoxOption, m_widget, {ElementString::Indicator});
    if (!m_indicatorElementList.isEmpty()) {
        m_indicatorProperties = queryProperties(m_indicatorElementList);
        if (m_indicatorProperties->icon()) {
            setIndicator(m_style->unionIcon(m_indicatorProperties, QString()));
        }
        if (m_indicatorProperties->layout()) {
            m_spacing = m_indicatorProperties->layout()->spacing().value_or(1);
        }
    }

    setIcon(m_comboBoxOption->currentIcon);
    setText(m_comboBoxOption->currentText);
    updateSubElementList();
    layout();
}

ComboBoxElement::~ComboBoxElement()
{
}

void ComboBoxElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_ComboBoxLabel:
        drawIcon(painter);
        if (!isEditable()) {
            drawText(painter);
        }
        break;
    }

    switch (enums.ComplexControl) {
    case QStyle::CC_ComboBox:
        drawBackground(painter);
        drawIcon(painter);
        if (!isEditable()) {
            drawText(painter);
        }
        drawIndicator(painter);
        break;
    }
}

bool ComboBoxElement::isEditable() const
{
    return m_editable;
}

void ComboBoxElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(ElementString::Indicator);
    if (hasText()) {
        m_subElementList.append(ElementString::Text);
    }
    if (hasIcon()) {
        m_subElementList.append(ElementString::Icon);
    }
}

QSizeF ComboBoxElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    QRectF rect = unifiedRect(m_layoutMap);
    // Follow the contents width
    rect.setWidth(contentsSizeFromStyle.width());
    auto size = applyPaddingToSize(rect.size());
    if (m_indicatorProperties && m_indicatorProperties->layout()) {
        size.rwidth() += m_indicatorProperties->layout()->spacing().value_or(20);
    }
    return size;
}

QRectF ComboBoxElement::subControlRect(QStyle::SubControl subControl) const
{
    if (!m_isValid) {
        qCWarning(UNION_QTWIDGETS) << "subControlRect for " << subControl << "is not valid";
        return QRect();
    }

    switch (subControl) {
    case QStyle::SC_ComboBoxFrame:
        return backgroundRectangle(m_comboBoxOption, m_backgroundProperties).toRect();
    case QStyle::SC_ComboBoxListBoxPopup:
        return m_comboBoxOption->rect;

    case QStyle::SC_ComboBoxArrow: {
        auto map = layoutMap(m_backgroundElementList, m_comboBoxOption, {ElementString::Indicator});
        auto rect = map[ElementString::Indicator].rect;
        rect = rect.adjusted(-m_spacing, 0, m_spacing, 0);
        return m_style->visualRect(m_comboBoxOption->direction, m_comboBoxOption->rect, rect.toRect());
    }

    case QStyle::SC_ComboBoxEditField: {
        QRectF labelRect;
        auto rect = m_comboBoxOption->rect;
        auto indicatorRect = subControlRect(QStyle::SC_ComboBoxArrow);
        labelRect = QRect(rect.left(), rect.top(), rect.width() - indicatorRect.width(), rect.height());
        // Add some spacing between the icon and text in edit field
        if (m_backgroundProperties->layout() && !m_comboBoxOption->currentIcon.isNull()) {
            auto spacing = m_backgroundProperties->layout()->spacing().value_or(5);
            labelRect.adjust(spacing, 0, spacing, 0);
        }
        return m_style->visualRect(m_comboBoxOption->direction, m_comboBoxOption->rect, labelRect.toRect());
    }

    default:
        break;
    }
    return QRect();
}

QStringList ComboBoxElement::elementHints() const
{
    QStringList hints;
    if (!m_comboBoxOption->frame) {
        hints.append(u"flat"_s);
    }
    if (m_comboBoxOption->editable) {
        hints.append(u"editable"_s);
    }
    return hints;
}

void ComboBoxElement::drawText(QPainter *painter) const
{
    if (m_editable) {
        return;
    }
    AbstractElement::drawText(painter);
}

qreal ComboBoxElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ComboBoxFrameWidth:
        return averageBorderSize();
    default:
        break;
    }
    return 0;
}
