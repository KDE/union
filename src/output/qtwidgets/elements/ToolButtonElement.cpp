// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ToolButtonElement.h"

#include <QToolButton>

#include "IconSubElement.h"
#include "IndicatorSubElement.h"
#include "TextSubElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;
using namespace Union::Widgets;

static OptionToElementRegistration<ToolButtonElement, QStyleOption::SO_ToolButton> toolButtonOptionRegistration;
static WidgetToElementRegistration<ToolButtonElement, QToolButton> toolButtonWidgetRegistration;

ToolButtonElement::ToolButtonElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
}

void ToolButtonElement::update(const QStyleOption *option)
{
    auto e = element();
    if (!e) {
        e = Union::Element::create();
        e->setType({ElementString::ToolButton});
        setElement(e);
    }

    const auto toolOption = qstyleoption_cast<const QStyleOptionToolButton *>(option);
    if (!toolOption) {
        return AbstractElement::update(option);
    }

    e->setHint(u"raised"_s, !option->state.testFlag(QStyle::State_AutoRaise));
    e->setHint(u"menu-open"_s, option->state.testFlags({QStyle::State_Sunken, QStyle::State_Raised}));

    const bool hasIndicator = toolOption->features.testFlag(QStyleOptionToolButton::HasMenu) || toolOption->features.testFlag(QStyleOptionToolButton::Menu);
    const bool hasPopupMenu = (hasIndicator && toolOption->features.testFlag(QStyleOptionToolButton::MenuButtonPopup));
    const bool hasInlineIndicator = (hasIndicator && !hasPopupMenu);
    const bool hasDelayedMenu = (hasInlineIndicator && toolOption->features.testFlag(QStyleOptionToolButton::PopupDelay));
    const bool hasIcon = !toolOption->icon.isNull() || (toolOption->features.testFlag(QStyleOptionToolButton::Arrow));
    const bool iconOnly = toolOption->toolButtonStyle == Qt::ToolButtonIconOnly || (toolOption->text.isEmpty() && hasIcon);

    if (hasPopupMenu) {
        m_arrowStyle = ArrowStyle::MenuButton;
    } else if (hasDelayedMenu) {
        m_arrowStyle = ArrowStyle::InlineArrow;
    } else if (hasInlineIndicator && !iconOnly) {
        m_arrowStyle = ArrowStyle::Menu;
    } else {
        m_arrowStyle = ArrowStyle::None;
    }

    m_toolButtonStyle = toolOption->toolButtonStyle;
    switch (m_toolButtonStyle) {
    case Qt::ToolButtonIconOnly:
        e->setAttribute(u"display"_s, u"icon-only"_s);
        break;
    case Qt::ToolButtonTextOnly:
        e->setAttribute(u"display"_s, u"text-only"_s);
        break;
    case Qt::ToolButtonFollowStyle:
    case Qt::ToolButtonTextBesideIcon:
        e->setAttribute(u"display"_s, u"text-beside-icon"_s);
        break;
    case Qt::ToolButtonTextUnderIcon:
        e->setAttribute(u"display"_s, u"text-under-icon"_s);
        break;
    default:
        break;
    }

    if (hasIndicator) {
        firstChildOfType<IndicatorSubElement>(true);
    } else {
        removeChildElement(firstChildOfType<IndicatorSubElement>());
    }

    if (!toolOption->text.isEmpty() && m_toolButtonStyle != Qt::ToolButtonIconOnly) {
        firstChildOfType<TextSubElement>(true)->setText(toolOption->text);
    } else {
        removeChildElement(firstChildOfType<TextSubElement>());
    }

    if (!toolOption->icon.isNull() && m_toolButtonStyle != Qt::ToolButtonTextOnly) {
        auto iconElement = firstChildOfType<IconSubElement>(true);
        iconElement->setIcon(toolOption->icon);
        iconElement->setSize(toolOption->iconSize);
    } else {
        removeChildElement(firstChildOfType<TextSubElement>());
    }

    // updateGraphicsEffect(option->rect);

    AbstractElement::update(option);

    //     m_hasArrows = m_toolButtonOption->features.testFlag(QStyleOptionToolButton::Arrow) && m_toolButtonOption->toolButtonStyle != Qt::ToolButtonTextOnly;
}

// void ToolButtonElement::layout()
// {
//     m_indicatorElementList = prepareElements(option, widget(), {ElementString::Indicator});
//     setIndicator(QIcon());
//     if (!m_indicatorElementList.isEmpty()) {
//         m_indicatorProperties = queryProperties(m_indicatorElementList);
//         setIndicator(style()->unionIcon(m_indicatorProperties, QString()));
//     }
//
//     m_elementList = prepareElements(option, widget());
//     if (!m_elementList.isEmpty()) {
//         m_elementProperties = queryProperties(m_elementList);
//         m_indicatorMap = layoutMap(option, {ElementString::Indicator});
//         layoutButtons();
//         // Update layoutmap so that the text and icon are within the main button
//         if (m_toolButtonOption) {
//             auto subopt = *m_toolButtonOption;
//             subopt.rect = m_mainButtonRect.toRect();
//             m_layoutMap = layoutMap(&subopt, m_subElementList);
//         } else {
//             m_layoutMap = layoutMap(option, m_subElementList);
//         }
//         m_isValid = true;
//     } else {
//         m_isValid = false;
//         qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for ToolButtonElement!";
//     }
// }

// void ToolButtonElement::updateSubElementList()
// {
//     m_subElementList.clear();
//     if (m_hasIcon || m_hasArrows) {
//         m_subElementList.append(ElementString::Icon);
//     }
//     if (m_hasText) {
//         m_subElementList.append(ElementString::Text);
//     }
// }

QSizeF ToolButtonElement::contentsSize([[maybe_unused]] const QStyleOption *option, QSizeF contentsSizeFromStyle) const
{
    QSizeF size = propertyWrapper().adjustSize(contentsSizeFromStyle, false);

    // auto indicator = firstChildOfType<IndicatorSubElement>();
    // if (indicator && m_arrowStyle == ArrowStyle::Menu) {
    //     auto diff = m_mainButtonRect.size() - size;
    //     size = (m_mainButtonRect | m_menuButtonRect).size() - diff;
    // }
    if (m_arrowStyle == ArrowStyle::MenuButton) {
        if (firstChildOfType<TextSubElement>()) {
            size.rwidth() += propertyWrapper().averageHPadding();
        }
        if (auto iconSubElement = firstChildOfType<IconSubElement>()) {
            size.rwidth() += iconSubElement->size().width() / 2.0;
        }
    }
    return size;
}

std::optional<QRectF> ToolButtonElement::subControlRect(const QStyleOption *option, QStyle::SubControl subControl) const
{
    if (subControl == QStyle::SC_ToolButton) {
        return rect();
        // return visualRect(m_mainButtonRect);
    }
    if (subControl == QStyle::SC_ToolButtonMenu) {
        // return visualRect(m_menuButtonRect);
    }

    return std::nullopt;
}

void ToolButtonElement::draw(QPainter *painter, DrawVariant what) const
{
    std::visit(overloaded{//
                          [](std::nullopt_t) { },
                          [&](QStyle::ComplexControl complexControl) {
                              if (complexControl == QStyle::CC_ToolButton) {
                                  drawBackground(painter);
                                  drawChildren(painter);
                              }
                          },
                          [&](QStyle::ControlElement controlElement) {
                              if (controlElement == QStyle::CE_ToolButtonLabel) {
                                  drawChildren<TextSubElement>(painter);
                              }
                          },
                          [&](QStyle::PrimitiveElement primitiveElement) {
                              switch (primitiveElement) {
                              case QStyle::PE_FrameButtonTool:
                                  drawBackground(painter, UnionPainter::Parts::FrameOnly);
                                  break;
                              case QStyle::PE_PanelButtonTool:
                                  drawBackground(painter);
                                  break;
                              default:
                                  break;
                              }
                          }},
               what);
}

// void ToolButtonElement::drawIcon(QPainter *painter) const
// {
//     if (m_toolButtonOption->toolButtonStyle == Qt::ToolButtonTextOnly) {
//         return;
//     }
//
//     QRectF iconRect = m_layoutMap[ElementString::Icon].rect;
//     if (m_toolButtonOption && m_toolButtonOption->features.testFlag(QStyleOptionToolButton::Arrow)) {
//         auto subopt = *m_toolButtonOption;
//         subopt.rect = iconRect.toRect();
//         switch (m_toolButtonOption->arrowType) {
//         case Qt::LeftArrow:
//             style()->drawPrimitive(QStyle::PE_IndicatorArrowLeft, &subopt, painter);
//             break;
//         case Qt::RightArrow:
//             style()->drawPrimitive(QStyle::PE_IndicatorArrowRight, &subopt, painter);
//             break;
//         case Qt::UpArrow:
//             style()->drawPrimitive(QStyle::PE_IndicatorArrowUp, &subopt, painter);
//             break;
//         case Qt::DownArrow:
//             style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &subopt, painter);
//             break;
//         default:
//             break;
//         }
//         return;
//     } else if (hasIcon()) {
//         drawIconAtRect(painter, m_icon, iconRect);
//     }
// }
//
// void ToolButtonElement::drawIndicator(QPainter *painter) const
// {
//     if (!m_hasIndicator) {
//         return;
//     }
//     auto rect = subControlRect(QStyle::SC_ToolButtonMenu);
//     auto indicatorRect = m_indicatorMap[ElementString::Indicator].rect;
//     indicatorRect.moveCenter(rect.center());
//     UnionPainter unionPainter(painter, m_indicatorProperties);
//     unionPainter.setRect(rect);
//     unionPainter.draw();
//     drawIconAtRect(painter, m_indicator, visualRect(indicatorRect), m_indicatorProperties);
// }
//
//
// void ToolButtonElement::layoutButtons()
// {
//     if (!m_toolButtonOption) {
//         return;
//     }
//     m_mainButtonRect = m_toolButtonOption->rect;
//     if (!m_hasIndicator) {
//         m_menuButtonRect = QRectF();
//         return;
//     }
//     m_menuButtonRect = m_indicatorMap[ElementString::Indicator].rect;
//
//     // With inline arrow, the main element does not care about making room to it,
//     // as this arrow lives within the padding area.
//     bool arrowInline = (arrowStyle() == ArrowStyle::InlineArrow);
//     bool arrowMenu = (arrowStyle() == ArrowStyle::Menu);
//     // Align the second button around the main button
//     if (m_indicatorProperties) {
//         auto alignH = m_indicatorProperties->safePropertyLookup(Alignment::Unspecified,
//                                                                 &StylePropertyGroup::layout,
//                                                                 &LayoutPropertyGroup::alignment,
//                                                                 &AlignmentPropertyGroup::horizontal);
//         auto alignV = m_indicatorProperties->safePropertyLookup(Alignment::Unspecified,
//                                                                 &StylePropertyGroup::layout,
//                                                                 &LayoutPropertyGroup::alignment,
//                                                                 &AlignmentPropertyGroup::vertical);
//
//         const QMarginsF menuPadding = padding(m_indicatorProperties);
//         m_menuButtonRect = m_menuButtonRect.marginsAdded(menuPadding);
//         const QMarginsF elementPadding = padding();
//
//         switch (alignH) {
//         case Union::Properties::Alignment::Start:
//             // With regular Menu, ignore the paddings that would cause the button to grow too large
//             // as the menu is embedded within the button
//             if (arrowMenu) {
//                 m_menuButtonRect.setWidth(m_menuButtonRect.width() - menuPadding.right() - elementPadding.left());
//                 m_menuButtonRect.moveLeft(m_mainButtonRect.left() + elementPadding.left());
//             } else {
//                 m_menuButtonRect.moveLeft(m_mainButtonRect.left());
//             }
//             if (!arrowInline) {
//                 m_mainButtonRect.setLeft(m_menuButtonRect.right());
//             }
//             break;
//         case Union::Properties::Alignment::Center:
//             m_menuButtonRect.moveCenter(QPoint(m_mainButtonRect.center().x(), m_menuButtonRect.center().y()));
//             break;
//         case Union::Properties::Alignment::End:
//         case Union::Properties::Alignment::Unspecified:
//         case Union::Properties::Alignment::StackCenter:
//             if (arrowMenu) {
//                 m_menuButtonRect.setWidth(m_menuButtonRect.width() - menuPadding.left() - elementPadding.right());
//                 m_menuButtonRect.moveRight(m_mainButtonRect.right() - elementPadding.right());
//             } else {
//                 m_menuButtonRect.moveRight(m_mainButtonRect.right());
//             }
//             if (!arrowInline) {
//                 m_mainButtonRect.setRight(m_menuButtonRect.left());
//             }
//             break;
//         case Union::Properties::Alignment::StackFill:
//         case Union::Properties::Alignment::Fill:
//             m_menuButtonRect.setRight(m_mainButtonRect.right());
//             m_menuButtonRect.setLeft(m_mainButtonRect.left());
//             break;
//         }
//
//         switch (alignV) {
//         case Union::Properties::Alignment::Start:
//             m_menuButtonRect.moveTop(m_mainButtonRect.top());
//             if (!arrowInline) {
//                 m_mainButtonRect.setTop(m_menuButtonRect.bottom());
//             }
//             break;
//         case Union::Properties::Alignment::Center:
//         case Union::Properties::Alignment::Unspecified:
//         case Union::Properties::Alignment::StackCenter:
//             m_menuButtonRect.moveCenter(QPoint(m_menuButtonRect.center().x(), m_mainButtonRect.center().y()));
//             break;
//         case Union::Properties::Alignment::End:
//             m_menuButtonRect.moveBottom(m_mainButtonRect.bottom());
//             if (!arrowInline) {
//                 m_mainButtonRect.setBottom(m_menuButtonRect.top());
//             }
//             break;
//         case Union::Properties::Alignment::Fill:
//         case Union::Properties::Alignment::StackFill:
//             m_menuButtonRect.setTop(m_mainButtonRect.top());
//             m_menuButtonRect.setBottom(m_mainButtonRect.bottom());
//             break;
//         }
//     }
// }
//
