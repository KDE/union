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

        auto maxImplicitHeight = std::ranges::max(buckets() | std::views::transform([](auto bucket) {
                                                      return bucket->implicitSize.height();
                                                  }));
        qreal height = size.isEmpty() ? maxImplicitHeight : size.height();

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

        center.position = centerPosition + QPointF{(remainingSize.width() - center.size.width()) / 2.0, 0.0};

        // qDebug() << fill.size << center.size;

        auto maxCenter = QSizeF{std::max(center.size.width(), fill.size.width()), //
                                std::max(center.size.height(), fill.size.height())};

        size = QSizeF{spacedSize(spacing, start.size.width(), maxCenter.width(), end.size.width()),
                      std::max({start.size.height(), maxCenter.height(), end.size.height()})};

        for (auto bucket : buckets()) {
            qreal stackedY = bucket->stackCenter ? std::max(0.0, (bucket->size.height() - bucket->implicitSize.height()) / 2.0) : 0.0;
            qreal stackFillHeight = bucket->size.height() / bucket->items.count();
            qreal fillX = 0.0;
            qreal fillWidth = bucket->size.width() / bucket->items.count();

            for (auto &item : bucket->items) {
                item.size = item.implicitSize;

                if (bucket == &fill) {
                    fillX += item.margins.left();
                    item.position.setX(fillX);
                    item.size.setWidth(fillWidth - item.margins.left() - item.margins.right());
                    fillX += fillWidth;
                }

                switch (item.verticalAlignment) {
                case Union::Properties::Alignment::Unspecified:
                case Union::Properties::Alignment::Start:
                    item.position.setY(item.margins.top());
                    break;
                case Union::Properties::Alignment::Center:
                    item.position.setY(std::round(bucket->size.height() / 2 - (item.implicitSize.height() + item.margins.top() + item.margins.bottom()) / 2));
                    break;
                case Union::Properties::Alignment::End:
                    item.position.setY(bucket->size.height() - item.implicitSize.height() - item.margins.bottom());
                    break;
                case Union::Properties::Alignment::Fill:
                    item.position.setY(item.margins.top());
                    item.size.setHeight(bucket->size.height() - item.margins.top() - item.margins.bottom());
                    break;
                case Union::Properties::Alignment::StackCenter:
                    stackedY += item.margins.top();
                    item.position.setY(stackedY);
                    item.size.setWidth(bucket->size.width());
                    stackedY += item.implicitSize.height() + item.margins.bottom();
                    break;
                case Union::Properties::Alignment::StackFill:
                    stackedY += item.margins.top();
                    item.position.setY(stackedY);
                    item.size.setWidth(bucket->size.width());
                    item.size.setHeight(stackFillHeight - item.margins.top() - item.margins.bottom());
                    stackedY += stackFillHeight + item.margins.bottom();
                    break;
                }
            }
        }
    }

    void positionItems(QQuickItem *parent)
    {
        for (auto bucket : buckets()) {
            bucket->positionItems(parent);
        }
    }
};

}
}
