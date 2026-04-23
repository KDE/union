// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include "LayoutBucket.h"
#include "LayoutHelpers.h"

namespace Union
{
namespace Quick
{

struct LayoutContainer {
    QPointF position = QPointF{0.0, 0.0};
    QSizeF size = QSizeF{0.0, 0.0};
    QSizeF implicitSize = QSizeF{0.0, 0.0};
    qreal spacing = 0.0;

    LayoutBucket start;
    LayoutBucket center;
    LayoutBucket end;
    LayoutBucket fill;

    inline std::array<LayoutBucket *, 4> buckets()
    {
        return {&start, &center, &end, &fill};
    }

    inline bool isEmpty()
    {
        return std::ranges::all_of(buckets(), std::bind_front(std::ranges::equal_to(), true), &LayoutBucket::isEmpty);
    }

    void setSpacing(qreal _spacing)
    {
        spacing = _spacing;
        for (auto bucket : buckets()) {
            bucket->spacing = _spacing;
        }
    }

    void sort()
    {
        for (auto bucket : buckets()) {
            bucket->sort();
        }
    }

    void layout()
    {
        std::ranges::for_each(buckets(), &LayoutBucket::layout);

        const qreal height =
            std::ranges::max({size.height(), start.implicitSize.height(), center.implicitSize.height(), end.implicitSize.height(), fill.implicitSize.height()});

        std::ranges::for_each(buckets(), [height](LayoutBucket *bucket) {
            bucket->size = QSizeF{bucket->implicitSize.width(), height};
        });

        auto maxImplicitCenter = QSizeF{std::max(center.implicitSize.width(), fill.implicitSize.width()), //
                                        std::max(center.implicitSize.height(), fill.implicitSize.height())};

        implicitSize = QSizeF{spacedSize(spacing, start.implicitSize.width(), maxImplicitCenter.width(), end.implicitSize.width()),
                              std::max({start.implicitSize.height(), maxImplicitCenter.height(), end.implicitSize.height()})};

        if (size.isEmpty()) {
            size = implicitSize;
        }

        start.position = position;
        end.position = QPointF{position.x() + size.width() - end.implicitSize.width(), position.y()};

        qreal startSpace = start.size.width() > 0 ? start.size.width() + spacing : 0.0;
        qreal endSpace = end.size.width() > 0 ? end.size.width() + spacing : 0.0;

        QPointF centerPosition = position + QPointF{startSpace, 0.0};
        QSizeF remainingSize = size - QSizeF{startSpace + endSpace, 0.0};

        fill.position = centerPosition;
        fill.size = remainingSize;

        center.size = QSizeF{std::min(remainingSize.width(), center.size.width()), std::min(remainingSize.height(), center.size.height())};
        center.position = centerPosition + QPointF{(remainingSize.width() - center.size.width()) / 2.0, (remainingSize.height() - center.size.height()) / 2.0};

        for (auto bucket : buckets()) {
            qreal stackedY = bucket->stackCenter ? std::max(0.0, (bucket->size.height() - bucket->implicitSize.height()) / 2.0) : 0.0;
            qreal stackFillHeight = bucket->size.height() / bucket->items.count();

            qreal fillX = bucket->position.x();
            qreal fillWidth = bucket->size.width() / bucket->items.count();

            for (auto &item : bucket->items) {
                item.position = bucket->position + item.position;
                item.size = item.implicitSize;

                if (bucket == &fill) {
                    fillX += item.margins.left();
                    item.position.setX(fillX);
                    item.size.setWidth(fillWidth - item.margins.left() - item.margins.right());
                    fillX += fillWidth;
                }

                const qreal top = bucket->position.y() + item.margins.top();

                switch (item.verticalAlignment) {
                case Union::Properties::Alignment::Unspecified:
                case Union::Properties::Alignment::Start:
                    item.position.setY(top);
                    break;
                case Union::Properties::Alignment::Center:
                    item.position.setY(top + (bucket->size.height() - item.margins.top() - item.margins.bottom() - item.implicitSize.height()) / 2);
                    break;
                case Union::Properties::Alignment::End:
                    item.position.setY(bucket->position.y() + (bucket->size.height() - item.implicitSize.height() - item.margins.bottom()));
                    break;
                case Union::Properties::Alignment::Fill:
                    item.position.setY(top);
                    item.size.setHeight(bucket->size.height() - item.margins.top() - item.margins.bottom());
                    break;
                case Union::Properties::Alignment::StackCenter:
                    stackedY += item.margins.top();
                    item.size.setWidth(std::min(bucket->size.width(), item.size.width()));
                    item.position.setX(bucket->position.x() + (bucket->size.width() - item.size.width()) / 2);
                    item.position.setY(bucket->position.y() + stackedY);
                    stackedY += item.implicitSize.height() + item.margins.bottom();
                    break;
                case Union::Properties::Alignment::StackFill:
                    stackedY += item.margins.top();
                    item.position.setY(bucket->position.y() + stackedY);
                    item.size.setWidth(bucket->size.width());
                    item.size.setHeight(stackFillHeight - item.margins.top() - item.margins.bottom());
                    stackedY += stackFillHeight + item.margins.bottom();
                    break;
                }
            }
        }
    }

    void positionItems(QQuickItem *parent, Qt::LayoutDirection direction)
    {
        for (auto bucket : buckets()) {
            bucket->positionItems(parent, direction);
        }
    }
};

}
}
