// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "GroupBoxElement.h"

#include <QGroupBox>

#include "IndicatorSubElement.h"
#include "TextSubElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;
using namespace Union::Widgets;

static OptionToElementRegistration<GroupBoxElement, QStyleOption::SO_GroupBox> groupBoxOptionRegistration;
static WidgetToElementRegistration<GroupBoxElement, QGroupBox> groupBoxWidgetRegistration;

GroupBoxElement::GroupBoxElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
}

void GroupBoxElement::update(const QStyleOption *option)
{
    auto e = ensureElement();
    e->setType({ElementString::GroupBox});

    const auto groupOption = qstyleoption_cast<const QStyleOptionGroupBox *>(option);
    if (!groupOption) {
        return AbstractElement::update(option);
    }

    e->setHint(u"flat"_s, groupOption->features.testFlag(QStyleOptionFrame::Flat));
    e->setHint(u"rounded"_s, groupOption->features.testFlag(QStyleOptionFrame::Rounded));

    // This info is only in the widget itself
    m_isCheckable = widget() ? qobject_cast<const QGroupBox *>(widget())->isCheckable() : false;
    if (m_isCheckable) {
        firstChildOfType<IndicatorSubElement>(true);
    } else {
        removeChildElement(firstChildOfType<IndicatorSubElement>());
    }

    if (!groupOption->text.isEmpty() && groupOption->subControls & QStyle::SC_GroupBoxLabel) {
        firstChildOfType<TextSubElement>(true)->setText(groupOption->text);
    } else {
        removeChildElement(firstChildOfType<TextSubElement>());
    }

    AbstractElement::update(option);
}

void GroupBoxElement::draw(QPainter *painter, DrawVariant what) const
{
    std::visit(overloaded{//
                          [](std::nullopt_t) { },
                          [](QStyle::ControlElement) { },
                          [&](QStyle::ComplexControl) {
                              drawBackground(painter);
                              drawChildren(painter);
                          },
                          [&](QStyle::PrimitiveElement primitive) {
                              switch (primitive) {
                              case QStyle::PE_FrameGroupBox:
                                  drawBackground(painter, UnionPainter::Parts::FrameOnly);
                              default:
                                  break;
                              }
                          }},
               what);
}

QRectF GroupBoxElement::subControlRect(const QStyleOption *option, QStyle::SubControl subControl) const
{
    // QRectF finalRect;

    switch (subControl) {
    case QStyle::SC_GroupBoxLabel: {
        return firstChildOfType<TextSubElement>()->rect();
    }
    case QStyle::SC_GroupBoxContents: {
        auto textRect = firstChildOfType<TextSubElement>()->rect();
        QMarginsF pad = propertyWrapper().padding();
        QRectF frameRect = rect();
        frameRect = frameRect.adjusted(0, textRect.height(), 0, 0);
        frameRect.adjust(0, 0, 0, -pad.bottom());
        // const int fontMetricsHeight = hasText() ? 0 : m_groupBoxOption->fontMetrics.height();
        int indicatorHeight = 0;
        if (m_isCheckable) {
            indicatorHeight = firstChildOfType<IndicatorSubElement>()->rect().height();
        }
        const int topMargin = indicatorHeight; // std::max(indicatorHeight, fontMetricsHeight);
        return frameRect.adjusted(pad.left(), pad.top() + topMargin, -pad.bottom(), -pad.right());
    }
    case QStyle::SC_GroupBoxCheckBox: {
        if (m_isCheckable) {
            return firstChildOfType<IndicatorSubElement>()->rect();
        } else {
            return {};
        }
        // const auto checkItem = m_layoutMap[ElementString::Icon];
        // const auto textItem = m_layoutMap[ElementString::Text];
        // finalRect = checkItem.rect;
        // // Ensure we align on the same level as text
        // if (checkItem.verticalAlignment == textItem.verticalAlignment) {
        //     finalRect.moveCenter(QPointF(finalRect.center().x(), textItem.rect.center().y()));
        // }
        // break;
    }
    case QStyle::SC_GroupBoxFrame: {
        return rect();
    }
    default:
        break;
    }

    return style()->QCommonStyle::subControlRect(QStyle::CC_GroupBox, qstyleoption_cast<const QStyleOptionComplex *>(option), subControl, widget());
}

int GroupBoxElement::styleHint(const QStyleOption *option, QStyle::StyleHint styleHint) const
{
    return style()->QCommonStyle::styleHint(styleHint, option, widget());
}

qreal GroupBoxElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric metric) const
{
    switch (metric) {
    case QStyle::PM_LayoutLeftMargin:
        return propertyWrapper().padding().left();
    case QStyle::PM_LayoutRightMargin:
        return propertyWrapper().padding().right();
    case QStyle::PM_LayoutTopMargin:
        return propertyWrapper().padding().top();
    case QStyle::PM_LayoutBottomMargin:
        return propertyWrapper().padding().bottom();
    case QStyle::PM_LayoutHorizontalSpacing:
    case QStyle::PM_LayoutVerticalSpacing:
        return propertyWrapper().spacing();
    default:
        break;
    }

    return style()->QCommonStyle::pixelMetric(metric, option, widget());
}

// void GroupBoxElement::drawText(QPainter *painter) const
// {
//     if (m_groupBoxOption && (m_groupBoxOption->subControls & QStyle::SC_GroupBoxLabel) && hasText()) {
//         QRectF textRect = subControlRect(QStyle::SC_GroupBoxLabel);
//         drawTextAtRect(painter, m_text, visualRect(textRect));
//     }
// }
//
// void GroupBoxElement::drawIcon(QPainter *painter) const
// {
//     if (m_isCheckable && m_groupBoxOption) {
//         QStyleOptionButton checkbox;
//         checkbox.rect = subControlRect(QStyle::SC_GroupBoxCheckBox).toRect();
//         checkbox.state = m_groupBoxOption->state;
//         style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkbox, painter);
//     }
// }
