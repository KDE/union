// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "SplitterElement.h"

#include "SharedNames.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

SplitterElement::SplitterElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
}

void SplitterElement::update(const QStyleOption *option)
{
    auto e = ensureElement();
    e->setType(ElementString::Splitter);

    AbstractElement::update(option);
}

void SplitterElement::draw(QPainter *painter, DrawVariant what) const
{
    std::visit(overloaded{//
                          [](std::nullopt_t) { },
                          [](QStyle::ComplexControl) { },
                          [&](QStyle::ControlElement controlElement) {
                              if (controlElement == QStyle::CE_Splitter) {
                                  drawBackground(painter);
                              }
                          },
                          [&](QStyle::PrimitiveElement primitive) {
                              if (primitive == QStyle::PE_IndicatorDockWidgetResizeHandle) {
                                  drawBackground(painter);
                              }
                          }},
               what);
}

std::optional<qreal> SplitterElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_SplitterWidth:
    case QStyle::PM_DockWidgetSeparatorExtent:
        return propertyWrapper().width();
    default:
        break;
    }
    return std::nullopt;
}
