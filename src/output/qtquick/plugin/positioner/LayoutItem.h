// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QQuickItem>

#include <PropertiesTypes.h>

namespace Union
{
namespace Quick
{
struct LayoutItem {
    QPointF position = QPointF{0.0, 0.0};
    QSizeF size = QSizeF{0.0, 0.0};
    QSizeF implicitSize = QSizeF{0.0, 0.0};
    Union::Properties::Alignment verticalAlignment = Union::Properties::Alignment::Unspecified;
    int order = 0;
    QMarginsF margins;
    QQuickItem *item = nullptr;

    void setItemPosition(QQuickItem *parent, const QPointF &relativeTo)
    {
        auto itemPosition = relativeTo + position;
        auto mapped = parent->mapToItem(item->parentItem(), itemPosition);
        item->setX(mapped.x());
        item->setY(mapped.y());
        item->setWidth(size.width());
        item->setHeight(size.height());
    }
};

}
}
