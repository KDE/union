// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ComboBoxElement.h"

#include <QComboBox>

#include "TextSubElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

static OptionToElementRegistration<ComboBoxElement, QStyleOption::SO_ComboBox> comboBoxOptionRegistration;
static WidgetToElementRegistration<ComboBoxElement, QComboBox> comboBoxWidgetRegistration;

ComboBoxElement::ComboBoxElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
    , m_editable(false)
    , m_hasFrame(true)
{
}

void ComboBoxElement::update(const QStyleOption *option)
{
    auto e = element();
    if (!e) {
        e = Union::Element::create();
        e->setType(ElementString::ComboBox);
        setElement(e);
    }

    if (const auto comboOption = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
        m_editable = comboOption->editable;
        e->setHint(u"editable"_s, m_editable);
        m_hasFrame = comboOption->frame;
        e->setHint(u"flat"_s, !m_hasFrame);

        if (!comboOption->currentText.isEmpty()) {
            auto textSubElement = firstChildOfType<TextSubElement>();
            if (!textSubElement) {
                textSubElement = new TextSubElement(style(), widget());
                textSubElement->setFlag(AbstractElementFlag::DrawMnemonics);
                addChildElement(textSubElement);
            }
            textSubElement->setText(comboOption->currentText);
        }

        // setIndicator(QIcon());
        // m_indicatorElementList = prepareElements(m_comboBoxOption, m_widget, {ElementString::Indicator});
        // if (!m_indicatorElementList.isEmpty()) {
        //     m_indicatorProperties = queryProperties(m_indicatorElementList);
        //     if (m_indicatorProperties->icon()) {
        //         setIndicator(m_style->unionIcon(m_indicatorProperties, QString()));
        //     }
        // }

        // setIcon(m_comboBoxOption->currentIcon);
        // setText(m_comboBoxOption->currentText);
        // updateSubElementList();
        // Adding shadows to editable widgets seems to cause issues with the painter
        // if (!m_editable) {
        //     updateGraphicsEffect(m_comboBoxOption->rect);
        // }
    }
    // layout();
    AbstractElement::update(option);
}

void ComboBoxElement::draw(QPainter *painter, DrawVariant what) const
{
    std::visit(overloaded{//
                          [](std::nullopt_t) { },
                          [](QStyle::PrimitiveElement) { },
                          [&](QStyle::ControlElement controlElement) {
                              switch (controlElement) {
                              case QStyle::CE_ComboBoxLabel:
                                  // drawIcon(painter);
                                  if (!m_editable) {
                                      firstChildOfType<TextSubElement>()->draw(painter, std::nullopt);
                                  }
                                  break;
                              default:
                                  break;
                              }
                          },
                          [&](QStyle::ComplexControl complexControl) {
                              switch (complexControl) {
                              case QStyle::CC_ComboBox:
                                  if (m_hasFrame) {
                                      drawBackground(painter);
                                  } else {
                                      drawPanel(painter);
                                  }
                                  // drawIcon(painter);
                                  // drawIndicator(painter);
                                  break;
                              default:
                                  break;
                              }
                          }},
               what);
}

QSizeF ComboBoxElement::contentsSize(QSizeF contentsSizeFromStyle) const
{
    // Follow the contents width
    auto size = propertyWrapper().adjustSize(contentsSizeFromStyle, false);
    // const auto indicatorSize = applyPaddingToSize(m_layoutMap[ElementString::Indicator].rect.size());
    // size.rwidth() += indicatorSize.width() + spacing();
    return size;
}

QRectF ComboBoxElement::subControlRect(QStyle::SubControl subControl) const
{
    switch (subControl) {
    case QStyle::SC_ComboBoxFrame:
        return /*visualRect(*/ backgroundRectangle(rect(), properties()).toRect();
    case QStyle::SC_ComboBoxListBoxPopup:
        return rect();
        // return visualRect(m_comboBoxOption->rect);
    case QStyle::SC_ComboBoxArrow: {
        // auto rect = m_layoutMap[ElementString::Indicator].rect;
        // rect = rect.adjusted(-spacing(), 0, spacing(), 0);
        // return m_style->visualRect(m_comboBoxOption->direction, m_comboBoxOption->rect, rect);
    }
    case QStyle::SC_ComboBoxEditField: {
        QRectF r = rect().marginsRemoved(propertyWrapper().borderSize());
        // rect.setWidth(rect.width() - subControlRect(QStyle::SC_ComboBoxArrow).width());
        // When drawing the text editor, it has padding of its own.
        // Remove our own padding and rely on the text editor padding.
        // auto paddingAdjustment = padding().left() + (hasIcon() ? spacing() : 0);
        // rect.moveLeft(m_layoutMap[ElementString::Text].rect.left() - paddingAdjustment - m_layoutMap[ElementString::Icon].rect.width());
        return UnionStyle::visualRect(direction(), rect(), r);
    }
    default:
        break;
    }

    return style()->QCommonStyle::subControlRect(QStyle::CC_ComboBox, nullptr, subControl, widget());
}

qreal ComboBoxElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ComboBoxFrameWidth:
        return propertyWrapper().averageBorderSize();
    default:
        break;
    }
    return 0;
}
