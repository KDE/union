// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "GroupBoxElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QGroupBox>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

GroupBoxElement::GroupBoxElement(const QStyleOptionGroupBox *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_groupBoxOption(option)
    , m_isCheckable(false)
{
    update();
}

GroupBoxElement::~GroupBoxElement()
{
}

void GroupBoxElement::update()
{
    // This info is only in the widget itself
    m_isCheckable = false;
    if (auto groupBox = qobject_cast<const QGroupBox *>(m_widget)) {
        if (groupBox->isCheckable()) {
            m_isCheckable = true;
        }
    }

    setText(m_groupBoxOption->text);
    layout();
}

void GroupBoxElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.PrimitiveElement) {
    case QStyle::PE_FrameGroupBox:
        drawFrame(painter);
        break;
    }

    switch (enums.ComplexControl) {
    case QStyle::CC_GroupBox:
        drawBackground(painter);
        drawIcon(painter);
        drawText(painter);
        drawIndicator(painter);
        break;
    }
}

void GroupBoxElement::layout()
{
    // We only layout by background, m_contentElementList etc are ignored

    m_backgroundElementList = prepareElements(m_groupBoxOption, m_widget);
    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
        m_isValid = true;
    } else {
        m_isValid = false;
    }
}

QRectF GroupBoxElement::subControlRect(QStyle::SubControl subControl) const
{
    if (!m_isValid) {
        qCWarning(UNION_QTWIDGETS) << "subControlRect for " << subControl << "is not valid";
        return QRect();
    }

    QRectF finalRect;

    switch (subControl) {
    case QStyle::SC_GroupBoxLabel: {
        auto map = layoutMap(m_backgroundElementList, m_groupBoxOption, {ElementString::Text});
        finalRect = map[ElementString::Text].rect;
    } break;
    case QStyle::SC_GroupBoxContents: {
        auto map = layoutMap(m_backgroundElementList, m_groupBoxOption, {ElementString::GroupBox, ElementString::Text});
        auto textRect = map[ElementString::Text].rect;
        QMarginsF padding;
        QRectF frameRect = m_groupBoxOption->rect;
        frameRect = frameRect.adjusted(0, textRect.height(), 0, 0);
        if (m_backgroundProperties->layout()) {
            padding = m_backgroundProperties->layout()->padding()->toMargins().toMargins();
        }
        frameRect.adjust(0, 0, 0, -padding.bottom());
        const int fontMetricsHeight = hasText() ? 0 : m_groupBoxOption->fontMetrics.height();
        int indicatorHeight = 0;
        if (m_isCheckable) {
            indicatorHeight = m_style->pixelMetric(QStyle::PM_IndicatorHeight, m_groupBoxOption, m_widget);
        }
        const int topMargin = qMax(indicatorHeight, fontMetricsHeight);
        return frameRect.adjusted(padding.left(), padding.top() + topMargin, -padding.bottom(), -padding.right());
    } break;
    case QStyle::SC_GroupBoxCheckBox: {
        auto map = layoutMap(m_backgroundElementList, m_groupBoxOption, {ElementString::Icon});
        finalRect = map[ElementString::Icon].rect;
    } break;
    case QStyle::SC_GroupBoxFrame: {
        return m_groupBoxOption->rect;
    }
    default:
        break;
    }

    return m_style->visualRect(m_groupBoxOption->direction, m_groupBoxOption->rect, finalRect.toRect());
}

void GroupBoxElement::drawText(QPainter *painter) const
{
    if ((m_groupBoxOption->subControls & QStyle::SC_GroupBoxLabel) && hasText()) {
        QRectF textRect = subControlRect(QStyle::SC_GroupBoxLabel);
        int textFlags = Qt::AlignLeading | Qt::AlignVCenter;
        const bool enabled = m_groupBoxOption->state.testFlag(QStyle::State_Enabled);
        QColor penColor = m_groupBoxOption->palette.text().color();
        if (m_backgroundProperties->text()) {
            auto textColor = m_backgroundProperties->text()->color();
            if (textColor) {
                penColor = textColor->toQColor();
            }
            textFlags = textFlagsFromProperties(m_backgroundProperties, false);
        }
        painter->save();
        if (m_backgroundProperties->text() && m_backgroundProperties->text()->font().has_value()) {
            painter->setFont(m_backgroundProperties->text()->font().value());
        }
        painter->setPen(penColor);
        m_style->drawItemText(painter, textRect.toRect(), textFlags, m_groupBoxOption->palette, enabled, m_text);
        painter->restore();
    }
}

void GroupBoxElement::drawIcon(QPainter *painter) const
{
    if (m_isCheckable) {
        QStyleOptionButton checkbox;
        checkbox.rect = subControlRect(QStyle::SC_GroupBoxCheckBox).toRect();
        checkbox.state = m_groupBoxOption->state;
        m_style->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkbox, painter);
    }
}

QStringList GroupBoxElement::elementHints() const
{
    QStringList hints;
    if (m_groupBoxOption->features.testFlag(QStyleOptionFrame::Flat)) {
        hints.append(u"flat"_s);
    }
    if (m_groupBoxOption->features.testFlag(QStyleOptionFrame::Rounded)) {
        hints.append(u"rounded"_s);
    }
    return hints;
}
