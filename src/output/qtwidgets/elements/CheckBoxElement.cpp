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
    auto e = element();
    if (!e) {
        e = Union::Element::create();
        if (qobject_cast<QCheckBox *>(widget())) {
            e->setType(ElementString::CheckBox);
        } else {
            e->setType(ElementString::RadioButton);
        }
        setElement(e);
    }

    if (auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option); buttonOption) {
        e->setHint(u"with-icon"_s, !buttonOption->icon.isNull());

        if (buttonOption->state.testFlag(QStyle::State_NoChange)) {
            e->setHint(u"indeterminate"_s, true);
        } else {
            e->setHint(u"indeterminate"_s, false);
        }

        if (!buttonOption->text.isEmpty()) {
            firstChildOfType<TextSubElement>(true)->setText(buttonOption->text);
        }

        if (!buttonOption->icon.isNull()) {
            auto iconSubElement = firstChildOfType<IconSubElement>(true);
            iconSubElement->setIcon(buttonOption->icon);
            iconSubElement->setSize(buttonOption->iconSize);
        }

        // updateGraphicsEffect(m_buttonOption->rect);
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

QRectF CheckBoxElement::subElementRect(const QStyleOption *option, QStyle::SubElement element) const
{
    switch (element) {
    case QStyle::SE_CheckBoxIndicator:
    case QStyle::SE_RadioButtonIndicator:
        return m_indicatorElement->rect();
    default:
        break;
    }

    // if (element == QStyle::SE_CheckBoxIndicator) {
    //     if (m_buttonOption && (m_buttonOption->styleObject || m_widget)) {
    //         // The indicator is drawn as part of something
    //         return visualRect(m_layoutMap[ElementString::Indicator].rect);
    //     }
    // }
    // The indicator is drawn standalone (PE_IndicatorCheckBox for example)
    return style()->QCommonStyle::subElementRect(element, option, widget());
}

qreal CheckBoxElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
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
    default:
        break;
    }
    return style()->QCommonStyle::pixelMetric(pixelMetric, option, widget());
}
