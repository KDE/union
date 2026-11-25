// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "FocusElement.h"

#include <QFocusFrame>

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

static OptionToElementRegistration<FocusElement, QStyleOption::SO_FocusRect> focusOptionRegistration;
static WidgetToElementRegistration<FocusElement, QFocusFrame> buttonWidgetRegistration;

FocusElement::FocusElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
}

void FocusElement::update(const QStyleOption *option)
{
    auto e = ensureElement();
    e->setType({ElementString::FocusFrame});

    const auto focusOption = qstyleoption_cast<const QStyleOptionFocusRect *>(option);
    if (focusOption) {
        m_backgroundColor = focusOption->backgroundColor;

        e->setHint(u"focus-at-border"_s, focusOption->state.testFlag(QStyle::State_FocusAtBorder));

        // updateGraphicsEffect(focusOption->rect);
    }

    AbstractElement::update(option);
}

void FocusElement::draw(QPainter *painter, DrawVariant what) const
{
    if (std::holds_alternative<QStyle::PrimitiveElement>(what)) {
        switch (std::get<QStyle::PrimitiveElement>(what)) {
        case QStyle::PE_FrameFocusRect:
            drawBackground(painter);
            break;
        default:
            break;
        }
    }
}

qreal FocusElement::pixelMetric([[maybe_unused]] const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_FocusFrameVMargin:
        return propertyWrapper().averageVPadding();
    case QStyle::PM_FocusFrameHMargin:
        return propertyWrapper().averageHPadding();
    default:
        break;
    }
    return 0;
}

void FocusElement::drawBackground(QPainter *painter, UnionPainter::Parts parts) const
{
    UnionPainter unionPainter(painter, properties());
    unionPainter.setRect(rect());
    if (m_backgroundColor.isValid()) {
        unionPainter.setOverrideBackgroundColor(m_backgroundColor);
    }
    unionPainter.setParts(parts);
    unionPainter.draw();
}
