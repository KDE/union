// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ToolBarElement.h"
#include "BackgroundDrawing.h"
#include "StyleUtils.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

#include "SharedNames.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;

ToolBarElement::ToolBarElement(const QStyleOptionToolBar *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_toolBarOption(option)
{
    update();
}

ToolBarElement::~ToolBarElement()
{
}

void ToolBarElement::update()
{
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
    // Background and content is separate
    m_backgroundElementList = prepareElements(m_toolBarOption, m_widget);
    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
        m_layoutMap = layoutMap(m_backgroundElementList, m_toolBarOption, m_subElementList);
    }

    m_handleElementList = prepareElements(m_toolBarOption, m_widget, {ElementString::Handle});
    if (!m_handleElementList.isEmpty()) {
        m_handleProperties = queryProperties(m_handleElementList);
    }
    m_separatorElementList = prepareElements(m_toolBarOption, m_widget, {ElementString::Separator});
    if (!m_separatorElementList.isEmpty()) {
        m_separatorProperties = queryProperties(m_handleElementList);
    }
    m_extensionElementList = prepareElements(m_toolBarOption, m_widget, {ElementString::Extension});
    if (!m_extensionElementList.isEmpty()) {
        m_extensionProperties = queryProperties(m_extensionElementList);
    }

    m_contentElementList = prepareElements(m_toolBarOption, m_widget, m_subElementList);
    if (!m_contentElementList.isEmpty()) {
        m_contentProperties = queryProperties(m_contentElementList);
        m_isValid = true;
    } else {
        m_isValid = false;
        qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
    }
}

void ToolBarElement::drawHandle(QPainter *painter) const
{
    if (m_isValid && m_toolBarOption && m_handleProperties) {
        drawBackgroundRectangle(painter, m_toolBarOption->rect, m_handleProperties);
    }
}

void ToolBarElement::drawSeparator(QPainter *painter) const
{
    if (m_isValid && m_toolBarOption && m_separatorProperties) {
        drawBackgroundRectangle(painter, m_toolBarOption->rect, m_separatorProperties);
    }
}

void ToolBarElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(ElementString::Frame);
}

QStringList ToolBarElement::elementHints() const
{
    QStringList hints;

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

qreal ToolBarElement::separatorExtent() const
{
    if (m_isValid && m_toolBarOption && m_separatorProperties && m_separatorProperties->layout()) {
        switch (m_toolBarOption->toolBarArea) {
        case Qt::LeftToolBarArea:
        case Qt::RightToolBarArea:
            return m_separatorProperties->safePropertyLookup(0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::width);
        case Qt::TopToolBarArea:
        case Qt::BottomToolBarArea:
            return m_separatorProperties->safePropertyLookup(0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::height);
        default:
            return 0;
            break;
        }
    }
    return 0;
}

qreal ToolBarElement::handleExtent() const
{
    if (m_isValid && m_toolBarOption && m_handleProperties) {
        switch (m_toolBarOption->toolBarArea) {
        case Qt::LeftToolBarArea:
        case Qt::RightToolBarArea:
            return m_handleProperties->safePropertyLookup(0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::width);
        case Qt::TopToolBarArea:
        case Qt::BottomToolBarArea:
            return m_handleProperties->safePropertyLookup(0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::height);
        default:
            return 0;
            break;
        }
    }
    return 0;
}

qreal ToolBarElement::extensionExtent() const
{
    if (m_isValid && m_toolBarOption && m_extensionProperties) {
        switch (m_toolBarOption->toolBarArea) {
        case Qt::LeftToolBarArea:
        case Qt::RightToolBarArea:
            return m_extensionProperties->safePropertyLookup(0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::width);
        case Qt::TopToolBarArea:
        case Qt::BottomToolBarArea:
            return m_extensionProperties->safePropertyLookup(0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::height);
        default:
            return 0;
            break;
        }
    }
    return 0;
}

qreal ToolBarElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ToolBarSeparatorExtent:
        return separatorExtent();
    case QStyle::PM_ToolBarFrameWidth:
        return averageBorderSize();
    case QStyle::PM_ToolBarHandleExtent:
        return handleExtent();
    case QStyle::PM_ToolBarExtensionExtent:
        return extensionExtent();
    case QStyle::PM_ToolBarItemSpacing:
        return spacing();
    case QStyle::PM_ToolBarItemMargin:
        return averagePadding();
    case QStyle::PM_ToolBarIconSize:
        return iconSize().width();
    default:
        break;
    }
    return 0;
}
