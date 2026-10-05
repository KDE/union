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
    , m_editable(false)
    , m_hasFrame(true)
{
    update();
}

void ComboBoxElement::update()
{
    if (!m_styleOption) {
        return;
    }
    m_editable = m_comboBoxOption->editable;
    m_hasFrame = m_comboBoxOption->frame;

    setIndicator(QIcon());
    m_indicatorElementList = prepareElements(m_comboBoxOption, m_widget, {ElementString::Indicator});
    if (!m_indicatorElementList.isEmpty()) {
        m_indicatorProperties = queryProperties(m_indicatorElementList);
        if (m_indicatorProperties->icon()) {
            setIndicator(m_style->unionIcon(m_indicatorProperties, QString()));
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
        if (m_hasFrame) {
            drawBackground(painter);
        } else {
            drawPanel(painter);
        }
        drawIcon(painter);
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
    if (hasText() || m_editable) {
        m_subElementList.append(ElementString::Text);
    }
    if (hasIcon()) {
        m_subElementList.append(ElementString::Icon);
    }
    m_subElementList.append(ElementString::Indicator);
}

QSizeF ComboBoxElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    // Follow the contents width
    auto size = applyPaddingToSize(contentsSizeFromStyle);
    const auto indicatorSize = applyPaddingToSize(m_layoutMap[ElementString::Indicator].rect.size());
    size.rwidth() += indicatorSize.width() + spacing();
    return size;
}

QRectF ComboBoxElement::subControlRect(QStyle::SubControl subControl) const
{
    if (!m_isValid || !m_styleOption) {
        qCWarning(UNION_QTWIDGETS) << "subControlRect for " << subControl << "is not valid";
        return QRect();
    }

    switch (subControl) {
    case QStyle::SC_ComboBoxFrame:
        return backgroundRectangle(m_comboBoxOption, m_backgroundProperties).toRect();
    case QStyle::SC_ComboBoxListBoxPopup:
        return m_comboBoxOption->rect;
    case QStyle::SC_ComboBoxArrow: {
        auto rect = m_layoutMap[ElementString::Indicator].rect;
        rect = rect.adjusted(-spacing(), 0, spacing(), 0);
        return m_style->visualRect(m_comboBoxOption->direction, m_comboBoxOption->rect, rect);
    }
    case QStyle::SC_ComboBoxEditField: {
        QRectF rect = m_comboBoxOption->rect.marginsRemoved(borderSize().toMargins());
        rect.setWidth(rect.width() - subControlRect(QStyle::SC_ComboBoxArrow).width());
        // When drawing the text editor, it has padding of its own.
        // Remove our own padding and rely on the text editor padding.
        auto paddingAdjustment = padding().left() + (hasIcon() ? spacing() : 0);
        rect.moveLeft(m_layoutMap[ElementString::Text].rect.left() - paddingAdjustment - m_layoutMap[ElementString::Icon].rect.width());
        return m_style->visualRect(m_comboBoxOption->direction, m_comboBoxOption->rect, rect);
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
