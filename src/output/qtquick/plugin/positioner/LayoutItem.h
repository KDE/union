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

    void setItemPosition(QQuickItem *parent, Qt::LayoutDirection direction)
    {
        auto positionInParent = position;
        if (direction == Qt::RightToLeft) {
            auto parentSize = parent->boundingRect().size();
            positionInParent = QPointF{parentSize.width() - size.width() - position.x(), position.y()};
        }

        auto mapped = parent->mapToItem(item->parentItem(), positionInParent);
        item->setX(std::round(mapped.x()));
        item->setY(std::round(mapped.y()));
        item->setSize(QSizeF{std::ceil(size.width()), std::ceil(size.height())});
    }
};

}
}
