// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ToolTipElement.h"

#include <QToolTip>

#include "FrameElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

static NameToElementRegistration<ToolTipElement> toolTipRegistration{{"QTipLabel"}};

ToolTipElement::ToolTipElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
    widget->setAttribute(Qt::WA_TranslucentBackground);
}

void ToolTipElement::update(const QStyleOption *option)
{
    auto e = ensureElement();
    e->setType(ElementString::ToolTip);

    if (auto frameOption = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
        e->setHints(FrameElement::frameHintsFor(frameOption));
        e->setAttributes(FrameElement::frameAttributesFor(frameOption, widget()));
    }

    AbstractElement::update(option);
}

void ToolTipElement::draw(QPainter *painter, DrawVariant what) const
{
    if (std::holds_alternative<QStyle::PrimitiveElement>(what)) {
        if (std::get<QStyle::PrimitiveElement>(what) == QStyle::PE_PanelTipLabel) {
            drawBackground(painter);
        }
    }
}

std::optional<QRectF> Union::Widgets::ToolTipElement::subElementRect(const QStyleOption *option, QStyle::SubElement element) const
{
    switch (element) {
    case QStyle::SE_ShapedFrameContents:
        return rect().marginsRemoved(propertyWrapper().padding());
    case QStyle::SE_FrameLayoutItem:
        return QRectF{};
    default:
        break;
    }

    return std::nullopt;
}


std::optional<qreal> ToolTipElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ToolTipLabelFrameWidth:
        return propertyWrapper().averageBorderSize();
    default:
        break;
    }
    return std::nullopt;
}
