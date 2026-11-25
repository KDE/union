// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "MenuBarElement.h"

#include <QMenuBar>

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

static WidgetToElementRegistration<MenuBarElement, QMenuBar> menuBarWidgetRegistration;

MenuBarElement::MenuBarElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
}

void MenuBarElement::update(const QStyleOption *option)
{
    auto e = ensureElement();
    e->setType(ElementString::MenuBar);

    AbstractElement::update(option);
}

void MenuBarElement::draw(QPainter *painter, DrawVariant what) const
{
    std::visit(overloaded{//
                          [](std::nullopt_t) { },
                          [](QStyle::ComplexControl) { },
                          [&](QStyle::ControlElement) {
                              drawBackground(painter);
                          },
                          [&](QStyle::PrimitiveElement) {
                              drawBackground(painter);
                          }},
               what);
}

qreal MenuBarElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_MenuBarVMargin:
        return propertyWrapper().averageVPadding();
    case QStyle::PM_MenuBarHMargin:
        return propertyWrapper().averageHPadding();
    case QStyle::PM_MenuBarPanelWidth:
        return propertyWrapper().width();
    case QStyle::PM_MenuBarItemSpacing:
        return propertyWrapper().spacing();
    default:
        break;
    }
    return style()->QCommonStyle::pixelMetric(pixelMetric, option, widget());
}
