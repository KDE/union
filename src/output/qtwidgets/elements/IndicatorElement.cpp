// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "IndicatorElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

IndicatorElement::IndicatorElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_indicatorOption(option)
{
    m_listViewIconSize = querySize({ElementString::ListViewIconSize}).width();
    m_smallIconSize = querySize({ElementString::SmallIconSize}).width();
    m_iconViewIconSize = querySize({ElementString::IconViewIconSize}).width();
    m_largeIconSize = querySize({ElementString::LargeIconSize}).width();
    m_messageBoxIconSize = querySize({ElementString::MessageBoxIconSize}).width();
    m_textCursorWidth = querySize({ElementString::TextCursorWidth}).width();

    m_isValid = true;
}

IndicatorElement::~IndicatorElement()
{
}

void IndicatorElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.PrimitiveElement) {
    case QStyle::PE_IndicatorArrowLeft:
        drawElement(painter, u"arrow-left-symbolic"_s, {ElementString::IndicatorArrowLeft});
        return;
    case QStyle::PE_IndicatorArrowUp:
        drawElement(painter, u"arrow-up-symbolic"_s, {ElementString::IndicatorArrowUp});
        return;
    case QStyle::PE_IndicatorArrowRight:
        drawElement(painter, u"arrow-right-symbolic"_s, {ElementString::IndicatorArrowRight});
        return;
    case QStyle::PE_IndicatorArrowDown:
        drawElement(painter, u"arrow-down-symbolic"_s, {ElementString::IndicatorArrowDown});
        return;
    case QStyle::PE_IndicatorButtonDropDown:
        drawElement(painter, u"arrow-down-symbolic"_s, {ElementString::IndicatorButtonDropDown});
        return;
    }
}

void IndicatorElement::drawElement(QPainter *painter, const QString &defaultIconName, QStringList targetHierarchy) const
{
    auto properties = queryProperties(prepareElements(m_indicatorOption, m_widget, targetHierarchy));
    auto icon = m_style->unionIcon(properties, defaultIconName);
    drawIconAtRect(painter, icon, m_indicatorOption->rect, properties);
}

qreal IndicatorElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ListViewIconSize:
        return m_listViewIconSize;
    case QStyle::PM_SmallIconSize:
        return m_smallIconSize;
    case QStyle::PM_IconViewIconSize:
        return m_iconViewIconSize;
    case QStyle::PM_LargeIconSize:
        return m_largeIconSize;
    case QStyle::PM_MessageBoxIconSize:
        return m_messageBoxIconSize;
    case QStyle::PM_TextCursorWidth:
        return m_textCursorWidth;
    default:
        break;
    }
    return 0;
}
