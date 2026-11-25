// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "IndicatorElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

IndicatorElement::IndicatorElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
    m_listViewIconSize = querySize({ElementString::ListViewIconSize}).width();
    m_smallIconSize = querySize({ElementString::SmallIconSize}).width();
    m_iconViewIconSize = querySize({ElementString::IconViewIconSize}).width();
    m_largeIconSize = querySize({ElementString::LargeIconSize}).width();
    m_messageBoxIconSize = querySize({ElementString::MessageBoxIconSize}).width();
    m_textCursorWidth = querySize({ElementString::TextCursorWidth}).width();

    m_isValid = true;
}

void IndicatorElement::draw(QPainter *painter, DrawVariant what) const
{
    if (m_isValid) {
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
}

void IndicatorElement::drawElement(QPainter *painter, const QString &defaultIconName, const QStringList &targetHierarchy) const
{
    if (option) {
        auto properties = queryProperties(prepareElements(option, widget(), targetHierarchy));
        auto icon = style()->unionIcon(properties, defaultIconName);
        drawIconAtRect(painter, icon, visualRect(option->rect), properties);
    }
}

std::optional<qreal> IndicatorElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
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
    return std::nullopt;
}
