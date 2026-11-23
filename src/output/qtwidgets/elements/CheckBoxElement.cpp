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
            auto textSubElement = firstChildOfType<TextSubElement>();
            if (!textSubElement) {
                textSubElement = new TextSubElement(style(), widget());
                textSubElement->setFlag(AbstractElementFlag::DrawMnemonics);
                addChildElement(textSubElement);
            }
            textSubElement->setText(buttonOption->text);
        }
        // setIcon(m_buttonOption->icon);
        // setText(m_buttonOption->text);
        // updateSubElementList();
        // updateGraphicsEffect(m_buttonOption->rect);
    }

    AbstractElement::update(option);
}

void CheckBoxElement::layout()
{
    // m_indicatorElementList = prepareElements(m_styleOption, m_widget, {ElementString::CheckBox, ElementString::Indicator});
    // if (!m_indicatorElementList.isEmpty()) {
    //     m_indicatorProperties = queryProperties(m_indicatorElementList);
    // }
    AbstractElement::layout();
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
                                  firstChildOfType<TextSubElement>()->draw(painter, std::nullopt);
                                  // drawIndicator(painter);
                                  break;
                              case QStyle::CE_CheckBoxLabel:
                              case QStyle::CE_RadioButtonLabel:
                                  firstChildOfType<TextSubElement>()->draw(painter, std::nullopt);
                                  break;
                              default:
                                  break;
                              }
                          },
                          [&](QStyle::PrimitiveElement primitiveElement) {
                              switch (primitiveElement) {
                              case QStyle::PE_IndicatorCheckBox:
                                  // drawIndicator(painter);
                              case QStyle::PE_IndicatorRadioButton:
                                  break;
                              default:
                                  break;
                              }
                          }},
               what);
}

QSizeF CheckBoxElement::contentsSize(QSizeF contentsSizeFromStyle) const
{
    auto size = AbstractElement::contentsSize(contentsSizeFromStyle);
    // Ensure indicator size is taken into account with the label
    // size.rwidth() += spacing() + width(m_indicatorProperties);
    return size;
}

QRectF CheckBoxElement::subElementRect(QStyle::SubElement element) const
{
    switch (element) {
    case QStyle::SE_CheckBoxClickRect:
    case QStyle::SE_RadioButtonClickRect:
        return rect();
    case QStyle::SE_CheckBoxIndicator:
    case QStyle::SE_RadioButtonIndicator:
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
    return style()->QCommonStyle::subElementRect(element, nullptr, widget());
}

qreal CheckBoxElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_CheckBoxLabelSpacing:
    case QStyle::PM_RadioButtonLabelSpacing:
        return propertyWrapper().spacing();
    case QStyle::PM_IndicatorWidth:
    case QStyle::PM_ExclusiveIndicatorWidth:
        // return propertySize(m_elementProperties).width();
    case QStyle::PM_IndicatorHeight:
    case QStyle::PM_ExclusiveIndicatorHeight:
        // return propertySize(m_indicatorProperties).height();
    default:
        break;
    }
    return 0;
}
