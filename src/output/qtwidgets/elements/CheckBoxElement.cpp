// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "CheckBoxElement.h"

#include <QCheckBox>
#include <QRadioButton>

#include "IconSubElement.h"
#include "IndicatorSubElement.h"
#include "TextSubElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

static WidgetToElementRegistration<CheckBoxElement, QCheckBox, QRadioButton> checkBoxRegistration;

CheckBoxElement::CheckBoxElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
    m_indicatorElement = firstChildOfType<IndicatorSubElement>(true);
}

void CheckBoxElement::update(const QStyleOption *option)
{
    auto e = ensureElement();
    if (qobject_cast<QCheckBox *>(widget())) {
        e->setType(ElementString::CheckBox);
    } else {
        e->setType(ElementString::RadioButton);
    }

    if (auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option); buttonOption) {
        e->setHint(u"with-icon"_s, !buttonOption->icon.isNull());

        if (buttonOption->state.testFlag(QStyle::State_NoChange)) {
            e->setHint(u"indeterminate"_s, true);
        } else {
            e->setHint(u"indeterminate"_s, false);
        }

        withFirstChild<TextSubElement>(!buttonOption->text.isEmpty(), [&](auto element) {
            element->setText(buttonOption->text);
        });

        withFirstChild<IconSubElement>(!buttonOption->icon.isNull(), [&](auto element) {
            element->setIcon(buttonOption->icon);
            element->setSize(buttonOption->iconSize);
        });
    }

    AbstractElement::update(option);
}

void CheckBoxElement::draw(QPainter *painter, DrawVariant what) const
{
    std::visit(overloaded{//
                          [](std::nullopt_t) { },
                          [](QStyle::ComplexControl) { },
                          [&](QStyle::ControlElement controlElement) {
                              switch (controlElement) {
                              case QStyle::CE_CheckBox:
                              case QStyle::CE_RadioButton:
                                  drawBackground(painter);
                                  drawChildren(painter, std::nullopt);
                                  break;
                              case QStyle::CE_CheckBoxLabel:
                              case QStyle::CE_RadioButtonLabel:
                                  drawChildren<TextSubElement>(painter, std::nullopt);
                                  break;
                              default:
                                  break;
                              }
                          },
                          [&](QStyle::PrimitiveElement primitiveElement) {
                              switch (primitiveElement) {
                              case QStyle::PE_IndicatorCheckBox:
                              case QStyle::PE_IndicatorRadioButton:
                                  drawChildren<IndicatorSubElement>(painter, std::nullopt);
                                  break;
                              default:
                                  break;
                              }
                          }},
               what);
}

QSizeF CheckBoxElement::contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const
{
    auto size = AbstractElement::contentsSize(option, contentsSizeFromStyle);
    // Ensure indicator size is taken into account with the label
    size.rwidth() += propertyWrapper().spacing() + m_indicatorElement->rect().width();
    return size;
}

std::optional<QRectF> CheckBoxElement::subElementRect(const QStyleOption *option, QStyle::SubElement element) const
{
    switch (element) {
    case QStyle::SE_CheckBoxClickRect:
    case QStyle::SE_CheckBoxFocusRect:
    case QStyle::SE_CheckBoxContents:
    case QStyle::SE_RadioButtonClickRect:
    case QStyle::SE_RadioButtonFocusRect:
    case QStyle::SE_RadioButtonContents:
        return rect();
    case QStyle::SE_CheckBoxIndicator:
    case QStyle::SE_RadioButtonIndicator:
        return visualRect(m_indicatorElement);
    default:
        break;
    }

    // The indicator is drawn standalone (PE_IndicatorCheckBox for example)
    return std::nullopt;
}

std::optional<qreal> CheckBoxElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_CheckBoxLabelSpacing:
    case QStyle::PM_RadioButtonLabelSpacing:
        return propertyWrapper().spacing();
    case QStyle::PM_IndicatorWidth:
    case QStyle::PM_ExclusiveIndicatorWidth:
        return m_indicatorElement->propertyWrapper().width();
    case QStyle::PM_IndicatorHeight:
    case QStyle::PM_ExclusiveIndicatorHeight:
        return m_indicatorElement->propertyWrapper().height();
    case QStyle::PM_ButtonIconSize:
        return std::max(propertyWrapper().iconSize().width(), propertyWrapper().iconSize().height());
    default:
        break;
    }
    return std::nullopt;
}
