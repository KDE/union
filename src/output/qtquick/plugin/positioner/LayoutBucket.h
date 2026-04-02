// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include "LayoutItem.h"

namespace Union
{
namespace Quick
{

struct LayoutBucket {
    QPointF position = QPointF{0.0, 0.0};
    QSizeF size = QSizeF{0.0, 0.0};
    QSizeF implicitSize = QSizeF{0.0, 0.0};
    qreal spacing = 0.0;
    bool stackCenter = false;
    bool stackFill = false;
    QList<LayoutItem> items;

    bool isEmpty()
    {
        return items.isEmpty();
    }

    void sort()
    {
        std::ranges::stable_sort(items, std::ranges::less(), &LayoutItem::order);
    }

    void layout()
    {
        qreal x = 0.0;
        qreal maxWidth = 0.0;
        qreal maxHeight = 0.0;
        qreal totalHeight = 0.0;

        for (auto &item : items) {
            if (stackCenter || stackFill) {
                item.position.setX(0.0);
                totalHeight += item.margins.top() + item.margins.bottom() + item.implicitSize.height();
            } else {
                if (qFuzzyIsNull(item.implicitSize.width())) {
                    continue;
                }

                if (x > 0.0) {
                    x += spacing;
                }

                x += item.margins.left();
                item.position.setX(x);
                x += item.implicitSize.width() + item.margins.right();
            }

            maxWidth = std::max(maxWidth, item.margins.left() + item.implicitSize.width() + item.margins.right());
            maxHeight = std::max(maxHeight, item.margins.top() + item.implicitSize.height() + item.margins.bottom());
        }

        implicitSize.setWidth(std::ceil(std::max(stackCenter || stackFill ? maxWidth : x, 0.0)));
        implicitSize.setHeight(std::ceil(std::max(stackCenter || stackFill ? totalHeight : maxHeight, 0.0)));
    }

    void positionItems(QQuickItem *parent)
    {
        for (auto item : items) {
            item.setItemPosition(parent, position);
        }
    }
};

}
}
