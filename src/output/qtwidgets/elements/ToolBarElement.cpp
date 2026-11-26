// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ToolBarElement.h"

#include <QToolBar>

using namespace Qt::StringLiterals;
using namespace Union::Properties;
using namespace Union::Widgets;

static OptionToElementRegistration<ToolBarElement, QStyleOption::SO_ToolBar> toolBarOptionRegistration;
static WidgetToElementRegistration<ToolBarElement, QToolBar> toolBarWidgetRegistration;

ToolBarElement::ToolBarElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
}

void ToolBarElement::update(const QStyleOption *option)
{
    auto e = ensureElement();
    e->setType(ElementString::ToolBar);

    auto toolBarOption = qstyleoption_cast<const QStyleOptionToolBar *>(option);
    if (!toolBarOption) {
        return AbstractElement::update(option);
    }

    e->setHint(u"header"_s, toolBarOption->toolBarArea == Qt::TopToolBarArea);
    e->setHint(u"footer"_s, toolBarOption->toolBarArea == Qt::BottomToolBarArea);
    e->setHint(u"left"_s, toolBarOption->toolBarArea == Qt::LeftToolBarArea);
    e->setHint(u"right"_s, toolBarOption->toolBarArea == Qt::RightToolBarArea);

    AbstractElement::update(option);
}

void ToolBarElement::draw(QPainter *painter, DrawVariant what) const
{
    std::visit(overloaded{//
                          [](std::nullopt_t) { },
                          [](QStyle::ComplexControl) { },
                          [&](QStyle::ControlElement controlElement) {
                              if (controlElement == QStyle::CE_ToolBar) {
                                  drawBackground(painter);
                              }
                          },
                          [&](QStyle::PrimitiveElement primitive) {
                              switch (primitive) {
                              case QStyle::PE_PanelToolBar:
                                  drawBackground(painter);
                                  break;
                              case QStyle::PE_IndicatorToolBarHandle:
                                  break;
                              case QStyle::PE_IndicatorToolBarSeparator:
                                  break;
                              default:
                                  break;
                              }
                          }},
               what);
}

std::optional<qreal> ToolBarElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ToolBarSeparatorExtent:
        // return extent(m_separatorProperties);
        break;
    case QStyle::PM_ToolBarFrameWidth:
        return propertyWrapper().averageBorderSize();
    case QStyle::PM_ToolBarHandleExtent:
        // return extent(m_handleProperties);
        break;
    case QStyle::PM_ToolBarExtensionExtent:
        // return extent(m_extensionProperties);
        break;
    case QStyle::PM_ToolBarItemSpacing:
        return propertyWrapper().spacing();
    case QStyle::PM_ToolBarItemMargin:
        return propertyWrapper().averagePadding();
    case QStyle::PM_ToolBarIconSize:
        return propertyWrapper().iconSize().width();
    default:
        break;
    }
    return std::nullopt;
}

// void ToolBarElement::layout()
// {
//     // Background uses toolbarOption, separators etc use styleOption
//     m_elementList = prepareElements(option, widget());
//     if (!m_elementList.isEmpty()) {
//         m_elementProperties = queryProperties(m_elementList);
//         m_layoutMap = layoutMap(option, m_subElementList);
//     }
//
//     m_handleElementList = prepareElements(option, widget(), {ElementString::Handle});
//     if (!m_handleElementList.isEmpty()) {
//         m_handleProperties = queryProperties(m_handleElementList);
//     }
//     m_separatorElementList = prepareElements(option, widget(), {ElementString::Separator});
//     if (!m_separatorElementList.isEmpty()) {
//         m_separatorProperties = queryProperties(m_separatorElementList);
//     }
//     m_extensionElementList = prepareElements(option, widget(), {ElementString::Extension});
//     if (!m_extensionElementList.isEmpty()) {
//         m_extensionProperties = queryProperties(m_extensionElementList);
//     }
//
//     if (!m_elementList.isEmpty()) {
//         m_isValid = true;
//     } else {
//         m_isValid = false;
//         qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
//     }
// }

// QRectF ToolBarElement::handleSeparatorRect(qreal extent) const
// {
//     if (!option) {
//         return QRectF();
//     }
//     QRectF rect = option->rect;
//     if (m_toolBarOption) {
//         switch (m_toolBarOption->toolBarArea) {
//         case Qt::LeftToolBarArea:
//         case Qt::RightToolBarArea:
//         case Qt::ToolBarArea_Mask:
//         case Qt::NoToolBarArea:
//             rect = centerRect(rect, rect.width(), extent);
//             break;
//         case Qt::TopToolBarArea:
//         case Qt::BottomToolBarArea:
//             rect = centerRect(rect, extent, rect.height());
//             break;
//         }
//     } else {
//         if (rect.height() > rect.width()) {
//             rect = centerRect(rect, extent, rect.height());
//         } else {
//             rect = centerRect(rect, rect.width(), extent);
//         }
//     }
//     return rect;
// }

// void ToolBarElement::drawHandle(QPainter *painter) const
// {
//     if (m_isValid && option && m_handleProperties) {
//         UnionPainter unionPainter(painter, m_handleProperties);
//         unionPainter.setRect(handleSeparatorRect(extent(m_handleProperties)));
//         unionPainter.draw();
//     }
// }
//
// void ToolBarElement::drawSeparator(QPainter *painter) const
// {
//     if (m_isValid && option && m_separatorProperties) {
//         UnionPainter unionPainter(painter, m_separatorProperties);
//         unionPainter.setRect(handleSeparatorRect(extent(m_separatorProperties)));
//         unionPainter.draw();
//     }
// }

// qreal ToolBarElement::extent(Union::StylePropertyGroupPtr properties) const
// {
//     if (m_isValid && m_toolBarOption && properties) {
//         switch (m_toolBarOption->toolBarArea) {
//         case Qt::LeftToolBarArea:
//         case Qt::RightToolBarArea:
//             return width(properties);
//         case Qt::TopToolBarArea:
//         case Qt::BottomToolBarArea:
//             return height(properties);
//         default:
//             return 0;
//             break;
//         }
//     }
//     if (properties) {
//         return width(properties);
//     }
//     return 0;
// }
