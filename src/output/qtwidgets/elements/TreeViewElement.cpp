// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "TreeViewElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;
using namespace Union::Properties;

TreeViewElement::TreeViewElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_treeViewOption(option)
    , m_itemIsOpen(false)
{
    update();
}

TreeViewElement::~TreeViewElement()
{
}

void TreeViewElement::update()
{
    if (!m_styleOption->state.testFlag(QStyle::State_Children)) {
        m_isValid = false;
        return;
    }
    if (m_styleOption->state.testFlag(QStyle::State_Item)) {
        m_itemIsOpen = false;
        m_isValid = true;
    }
    if (m_styleOption->state.testFlag(QStyle::State_Open)) {
        m_itemIsOpen = true;
        m_isValid = true;
    }
    layout();
}

void TreeViewElement::layout()
{
    m_indicatorElementList = prepareElements(m_styleOption, m_widget, {ElementString::TreeViewDelegate, ElementString::Indicator});
    if (!m_indicatorElementList.isEmpty()) {
        m_indicatorProperties = queryProperties(m_indicatorElementList);
    }
}

void TreeViewElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }
    switch (enums.PrimitiveElement) {
    case QStyle::PE_IndicatorBranch:
        drawIndicator(painter);
        break;
    }
}

void TreeViewElement::drawIndicator(QPainter *painter) const
{
    // For some reason treeview related items can have null styleoption :(
    if (!m_isValid || !m_styleOption) {
        return;
    }
    if (!m_styleOption->state.testFlag(QStyle::State_Children)) {
        return;
    }
    const qreal width = m_indicatorProperties->safePropertyLookup(0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::width);
    const qreal height = m_indicatorProperties->safePropertyLookup(0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::height);
    QSizeF size(width, height);

    auto defaultIconName = m_itemIsOpen ? u"arrow-down-symbolic"_s : u"arrow-right-symbolic"_s;
    auto icon = m_style->unionIcon(m_indicatorProperties, defaultIconName);
    const bool enabled = m_styleOption->state.testFlag(QStyle::State_Enabled);

    const QPalette activePalette = m_styleOption->palette;
    const qreal dpr = painter->device() ? painter->device()->devicePixelRatioF() : qApp->devicePixelRatio();
    auto iconSize = m_styleOption->rect.size();
    const QPixmap pixmap = icon.pixmap(iconSize, dpr, enabled ? QIcon::Normal : QIcon::Disabled);

    painter->save();
    auto rect = centerRect(m_styleOption->rect, size.width(), size.height());
    m_style->drawItemPixmap(painter, rect.toRect(), Qt::AlignCenter, pixmap);
    painter->restore();
}

QStringList TreeViewElement::elementHints() const
{
    QStringList elementHints;
    if (m_styleOption->state.testFlag(QStyle::State_Open)) {
        elementHints.append(u"expanded"_s);
    }
    return elementHints;
}

qreal TreeViewElement::indentation() const
{
    return querySize({ElementString::TreeViewDelegate, ElementString::Indentation}).width();
}

qreal TreeViewElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_TreeViewIndentation:
        return indentation();
    default:
        break;
    }
    return 0;
}
