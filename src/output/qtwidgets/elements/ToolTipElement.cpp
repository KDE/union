// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ToolTipElement.h"

#include "FrameElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

ToolTipElement::ToolTipElement(const QStyleOption *option, const UnionStyle *style, QWidget *widget)
    : AbstractElement(option, style, widget)
{
    update(option);
}

void ToolTipElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    if (const auto newOpt = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
        m_frameOption = newOpt;
    }
    layout();
    if (option) {
        updateGraphicsEffect(option->rect);
    }
}

void ToolTipElement::draw(QPainter *painter, DrawVariant what) const
{
    if (!m_isValid) {
        return;
    }
    switch (enums.PrimitiveElement) {
    case QStyle::PE_PanelTipLabel:
        drawBackground(painter);
        break;
    }
}

void ToolTipElement::layout()
{
    m_elementList = prepareElements(option, widget(), {ElementString::ToolTip});
    if (!m_elementList.isEmpty()) {
        m_elementProperties = queryProperties(m_elementList);
        m_isValid = true;
    } else {
        m_isValid = false;
    }
}

Union::IdentifierVariantHash ToolTipElement::elementAttributes() const
{
    return FrameElement::frameAttributesFor(m_frameOption, widget());
}

Union::IdentifierList ToolTipElement::elementHints() const
{
    return FrameElement::frameHintsFor(m_frameOption);
}

qreal ToolTipElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ToolTipLabelFrameWidth:
        return averageBorderSize();
    default:
        break;
    }
    return 0;
}

QString ToolTipElement::fallbackElementName([[maybe_unused]] const QStyleOption *opt) const
{
    return ElementString::ToolTip;
}
