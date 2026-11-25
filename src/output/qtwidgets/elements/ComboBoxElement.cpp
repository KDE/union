// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ComboBoxElement.h"

#include <QComboBox>

#include "IconSubElement.h"
#include "IndicatorSubElement.h"
#include "TextSubElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

static OptionToElementRegistration<ComboBoxElement, QStyleOption::SO_ComboBox> comboBoxOptionRegistration;
static WidgetToElementRegistration<ComboBoxElement, QComboBox> comboBoxWidgetRegistration;

ComboBoxElement::ComboBoxElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
    m_indicatorElement = firstChildOfType<IndicatorSubElement>(true);
}

void ComboBoxElement::update(const QStyleOption *option)
{
    auto e = ensureElement();
    e->setType(ElementString::ComboBox);

    if (const auto comboOption = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
        m_editable = comboOption->editable;
        e->setHint(u"editable"_s, m_editable);
        m_hasFrame = comboOption->frame;
        e->setHint(u"flat"_s, !m_hasFrame);

        withFirstChild<TextSubElement>(!comboOption->currentText.isEmpty(), [&](auto element) {
            element->setText(comboOption->currentText);
        });

        withFirstChild<IconSubElement>(!comboOption->currentIcon.isNull(), [&](auto element) {
            element->setIcon(comboOption->currentIcon);
            element->setSize(comboOption->iconSize);
        });

        // Adding shadows to editable widgets seems to cause issues with the painter
        // if (!m_editable) {
        //     updateGraphicsEffect(m_comboBoxOption->rect);
        // }
    }

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
                                  drawChildren<IconSubElement>(painter);
                                  if (!m_editable) {
                                      drawChildren<TextSubElement>(painter);
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
                                      drawBackground(painter, UnionPainter::Parts::PanelOnly);
                                  }
                                  drawChildren(painter);
                                  break;
                              default:
                                  break;
                              }
                          }},
               what);
}

QSizeF ComboBoxElement::contentsSize([[maybe_unused]] const QStyleOption *option, QSizeF contentsSizeFromStyle) const
{
    // Follow the contents width
    auto size = propertyWrapper().adjustSize(contentsSizeFromStyle, false);
    size.rwidth() += propertyWrapper().spacing() + m_indicatorElement->rect().width();
    return size;
}

QRectF ComboBoxElement::subControlRect([[maybe_unused]] const QStyleOption *option, QStyle::SubControl subControl) const
{
    switch (subControl) {
    case QStyle::SC_ComboBoxFrame:
        return UnionStyle::visualRect(direction(), rect(), propertyWrapper().backgroundRectangle(rect()));
    case QStyle::SC_ComboBoxListBoxPopup:
        return rect();
    case QStyle::SC_ComboBoxArrow: {
        auto r = m_indicatorElement->rect();
        const auto spacing = propertyWrapper().spacing();
        r.adjust(-spacing, 0, spacing, 0);
        return UnionStyle::visualRect(direction(), rect(), r);
    }
    case QStyle::SC_ComboBoxEditField: {
        QRectF r = rect().marginsRemoved(propertyWrapper().borderSize());
        r.setWidth(r.width() - subControlRect(option, QStyle::SC_ComboBoxArrow).width());
        // When drawing the text editor, it has padding of its own.
        // Remove our own padding and rely on the text editor padding.
        auto paddingAdjustment = propertyWrapper().padding().left(); //+ (hasIcon() ? spacing() : 0);
        // rect.moveLeft(m_layoutMap[ElementString::Text].rect.left() - paddingAdjustment - m_layoutMap[ElementString::Icon].rect.width());
        return UnionStyle::visualRect(direction(), rect(), r);
    }
    default:
        break;
    }

    return style()->QCommonStyle::subControlRect(QStyle::CC_ComboBox, qstyleoption_cast<const QStyleOptionComplex *>(option), subControl, widget());
}

std::optional<qreal> ComboBoxElement::pixelMetric([[maybe_unused]] const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ComboBoxFrameWidth:
        return propertyWrapper().averageBorderSize();
    default:
        break;
    }
    return std::nullopt;
}
