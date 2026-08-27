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

TreeViewElement::TreeViewElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_treeViewOption(option)
{
    update();
}

TreeViewElement::~TreeViewElement()
{
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
    if (!m_treeViewOption) {
        return;
    }
    if (!m_treeViewOption->state.testFlag(QStyle::State_Children)) {
        return;
    }
    auto defaultIconName = QString();
    if (m_treeViewOption->state.testFlag(QStyle::State_Item)) {
        defaultIconName = u"arrow-right-symbolic"_s;
    }
    if (m_treeViewOption->state.testFlag(QStyle::State_Open)) {
        defaultIconName = u"arrow-down-symbolic"_s;
    }

    QSizeF size(1, 1);
    auto elements = prepareElements(m_treeViewOption, m_widget, {ElementString::TreeViewDelegate, ElementString::Indicator});
    auto properties = queryProperties(elements);
    if (properties && properties->layout()) {
        size = QSizeF(properties->layout()->width().value_or(0), properties->layout()->height().value_or(0));
    }
    auto icon = m_style->unionIcon(properties, defaultIconName);
    const bool enabled = m_treeViewOption->state.testFlag(QStyle::State_Enabled);

    const QPalette activePalette = m_treeViewOption->palette;
    const qreal dpr = painter->device() ? painter->device()->devicePixelRatioF() : qApp->devicePixelRatio();
    auto iconSize = m_treeViewOption->rect.size();
    const QPixmap pixmap = icon.pixmap(iconSize, dpr, enabled ? QIcon::Normal : QIcon::Disabled);

    painter->save();
    auto rect = centerRect(m_treeViewOption->rect, size.width(), size.height());
    m_style->drawItemPixmap(painter, rect.toRect(), Qt::AlignCenter, pixmap);
    painter->restore();
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
