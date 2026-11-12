// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ToolBarElement.h"

#include "SharedNames.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;
using namespace Union::Widgets;

ToolBarElement::ToolBarElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
{
    setFlag(AbstractElementFlag::DrawMnemonics);
    update(option);
}

ToolBarElement::~ToolBarElement()
{
}

void ToolBarElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    if (const auto newOpt = qstyleoption_cast<const QStyleOptionToolBar *>(option)) {
        m_toolBarOption = newOpt;
    }
    updateSubElementList();
    layout();
}

void ToolBarElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_ToolBar:
        drawBackground(painter);
        break;
    }

    switch (enums.PrimitiveElement) {
    case QStyle::PE_PanelToolBar:
        drawBackground(painter);
        break;
    case QStyle::PE_IndicatorToolBarHandle:
        drawHandle(painter);
        break;
    case QStyle::PE_IndicatorToolBarSeparator:
        drawSeparator(painter);
        break;
    }
}

void ToolBarElement::layout()
{
    // Background uses toolbarOption, separators etc use styleOption
    m_elementList = prepareElements(m_styleOption, m_widget);
    if (!m_elementList.isEmpty()) {
        m_elementProperties = queryProperties(m_elementList);
        m_layoutMap = layoutMap(m_styleOption, m_subElementList);
    }

    m_handleElementList = prepareElements(m_styleOption, m_widget, {ElementString::Handle});
    if (!m_handleElementList.isEmpty()) {
        m_handleProperties = queryProperties(m_handleElementList);
    }
    m_separatorElementList = prepareElements(m_styleOption, m_widget, {ElementString::Separator});
    if (!m_separatorElementList.isEmpty()) {
        m_separatorProperties = queryProperties(m_separatorElementList);
    }
    m_extensionElementList = prepareElements(m_styleOption, m_widget, {ElementString::Extension});
    if (!m_extensionElementList.isEmpty()) {
        m_extensionProperties = queryProperties(m_extensionElementList);
    }

    if (!m_elementList.isEmpty()) {
        m_isValid = true;
    } else {
        m_isValid = false;
        qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
    }
}

QRectF ToolBarElement::handleSeparatorRect(qreal extent) const
{
    if (!m_styleOption) {
        return QRectF();
    }
    QRectF rect = m_styleOption->rect;
    if (m_toolBarOption) {
        switch (m_toolBarOption->toolBarArea) {
        case Qt::LeftToolBarArea:
        case Qt::RightToolBarArea:
        case Qt::ToolBarArea_Mask:
        case Qt::NoToolBarArea:
            rect = centerRect(rect, rect.width(), extent);
            break;
        case Qt::TopToolBarArea:
        case Qt::BottomToolBarArea:
            rect = centerRect(rect, extent, rect.height());
            break;
        }
    } else {
        if (rect.height() > rect.width()) {
            rect = centerRect(rect, extent, rect.height());
        } else {
            rect = centerRect(rect, rect.width(), extent);
        }
    }
    return rect;
}

void ToolBarElement::drawHandle(QPainter *painter) const
{
    if (m_isValid && m_styleOption && m_handleProperties) {
        UnionPainter unionPainter(painter, m_handleProperties);
        unionPainter.setRect(handleSeparatorRect(extent(m_handleProperties)));
        unionPainter.draw();
    }
}

void ToolBarElement::drawSeparator(QPainter *painter) const
{
    if (m_isValid && m_styleOption && m_separatorProperties) {
        UnionPainter unionPainter(painter, m_separatorProperties);
        unionPainter.setRect(handleSeparatorRect(extent(m_separatorProperties)));
        unionPainter.draw();
    }
}

void ToolBarElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(ElementString::Frame);
}

Union::IdentifierList ToolBarElement::elementHints() const
{
    Union::IdentifierList hints;
    if (!m_toolBarOption) {
        return hints;
    }

    switch (m_toolBarOption->toolBarArea) {
    case Qt::TopToolBarArea:
        hints.append(u"header"_s);
        break;
    case Qt::BottomToolBarArea:
        hints.append(u"footer"_s);
        break;
    case Qt::LeftToolBarArea:
        hints.append(u"left"_s);
        break;
    case Qt::RightToolBarArea:
        hints.append(u"right"_s);
        break;
    case Qt::ToolBarArea_Mask:
    case Qt::NoToolBarArea:
        break;
    }

    return hints;
}

qreal ToolBarElement::extent(Union::StylePropertyGroupPtr properties) const
{
    if (m_isValid && m_toolBarOption && properties) {
        switch (m_toolBarOption->toolBarArea) {
        case Qt::LeftToolBarArea:
        case Qt::RightToolBarArea:
            return width(properties);
        case Qt::TopToolBarArea:
        case Qt::BottomToolBarArea:
            return height(properties);
        default:
            return 0;
            break;
        }
    }
    if (properties) {
        return width(properties);
    }
    return 0;
}

qreal ToolBarElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ToolBarSeparatorExtent:
        return extent(m_separatorProperties);
    case QStyle::PM_ToolBarFrameWidth:
        return averageBorderSize(m_elementProperties);
    case QStyle::PM_ToolBarHandleExtent:
        return extent(m_handleProperties);
    case QStyle::PM_ToolBarExtensionExtent:
        return extent(m_extensionProperties);
    case QStyle::PM_ToolBarItemSpacing:
        return spacing();
    case QStyle::PM_ToolBarItemMargin:
        return averagePadding(m_elementProperties);
    case QStyle::PM_ToolBarIconSize:
        return iconSize(m_elementProperties).width();
    default:
        break;
    }
    return 0;
}

QString ToolBarElement::fallbackElementName([[maybe_unused]] const QStyleOption *opt) const
{
    return ElementString::ToolBar;
}
