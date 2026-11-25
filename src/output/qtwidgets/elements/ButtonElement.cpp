// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ButtonElement.h"

#include <QPushButton>

#include "IconSubElement.h"
#include "IndicatorSubElement.h"
#include "TextSubElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;
using namespace Union::Properties;

static OptionToElementRegistration<ButtonElement, QStyleOption::SO_Button> buttonOptionRegistration;
static WidgetToElementRegistration<ButtonElement, QPushButton> buttonWidgetRegistration;

ButtonElement::ButtonElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
}

void ButtonElement::update(const QStyleOption *option)
{
    auto e = ensureElement();
    e->setType(ElementString::Button);

    auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option);
    if (!buttonOption) {
        return AbstractElement::update(option);
    }

    if (buttonOption->features.testFlag(QStyleOptionButton::ButtonFeature::DefaultButton)
        || buttonOption->features.testFlag(QStyleOptionButton::ButtonFeature::AutoDefaultButton)) {
        e->setState(Union::Element::State::Highlighted, true);
    }

    e->setHint(u"with-icon"_s, !buttonOption->icon.isNull());
    e->setHint(u"flat"_s, buttonOption->features.testFlag(QStyleOptionButton::Flat));
    e->setHint(u"with-menu"_s, buttonOption->features.testFlag(QStyleOptionButton::HasMenu));
    e->setHint(u"command-link-button"_s, buttonOption->features.testFlag(QStyleOptionButton::CommandLinkButton));

    withFirstChild<TextSubElement>(!buttonOption->text.isEmpty(), [&](auto element) {
        element->setText(buttonOption->text);
    });

    withFirstChild<IconSubElement>(!buttonOption->icon.isNull(), [&](auto element) {
        element->setIcon(buttonOption->icon);
        element->setSize(buttonOption->iconSize);
    });

    withFirstChild<IndicatorSubElement>(buttonOption->features.testFlag(QStyleOptionButton::HasMenu), [](auto) { });

    AbstractElement::update(option);
}

void ButtonElement::draw(QPainter *painter, DrawVariant what) const
{
    std::visit(overloaded{//
                          [](std::nullopt_t) { },
                          [this, painter](QStyle::ControlElement element) {
                              switch (element) {
                              case QStyle::CE_PushButtonBevel:
                                  drawBackground(painter);
                                  break;
                              case QStyle::CE_PushButtonLabel:
                                  drawChildren<TextSubElement>(painter, std::nullopt);
                                  break;
                              case QStyle::CE_PushButton:
                                  drawBackground(painter);
                                  drawChildren(painter, std::nullopt);
                                  break;
                              default:
                                  break;
                              }
                          },
                          [](QStyle::ComplexControl) { },
                          [this, painter](QStyle::PrimitiveElement primitive) {
                              switch (primitive) {
                              case QStyle::PE_FrameButtonBevel:
                              case QStyle::PE_FrameDefaultButton:
                                  drawBackground(painter, UnionPainter::Parts::FrameOnly);
                                  break;
                              case QStyle::PE_PanelButtonCommand:
                              case QStyle::PE_PanelButtonBevel:
                                  drawBackground(painter);
                                  break;
                              default:
                                  break;
                              }
                          }},
               what);
}

QRectF ButtonElement::subElementRect([[maybe_unused]] const QStyleOption *option, QStyle::SubElement element) const
{
    if (element == QStyle::SE_PushButtonBevel || element == QStyle::SE_PushButtonFocusRect) {
        return UnionStyle::visualRect(direction(), rect(), propertyWrapper().backgroundRectangle(rect()));
    }

    return style()->QCommonStyle::subElementRect(element, nullptr, widget());
}

QSizeF ButtonElement::contentsSize([[maybe_unused]] const QStyleOption *option, QSizeF contentsSizeFromStyle) const
{
    auto size = AbstractElement::contentsSize(option, contentsSizeFromStyle);
    // Since text and icon are parts of background, we need to apply the indicator width and spacing from background
    // to get the proper contentSize
    auto indicator = firstChildOfType<IndicatorSubElement>();
    if (indicator) {
        size.rwidth() += indicator->rect().width() + propertyWrapper().spacing();
    }
    return size;
}

qreal ButtonElement::pixelMetric([[maybe_unused]] const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ButtonMargin:
        return propertyWrapper().averagePadding();
    case QStyle::PM_ButtonDefaultIndicator:
        return propertyWrapper().averageBorderSize();
    case QStyle::PM_ButtonIconSize:
        return propertyWrapper().iconSize().width();
    default:
        break;
    }
    return 0;
}
