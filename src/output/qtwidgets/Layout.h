// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QRectF>

#include "PropertiesTypes.h"

namespace Union
{
namespace Widgets
{

class AbstractElement;

struct Layout {
    enum class BucketType {
        Start,
        Center,
        End,
        Fill
    };

    struct LayoutItem {
        // QString elementName;
        AbstractElement *element = nullptr;
        int order;
        Union::Properties::Alignment horizontalAlignment;
        Union::Properties::Alignment verticalAlignment;
        QRectF rect;
    };

    struct LayoutBucket {
        BucketType type;
        QRectF rect;
        QList<LayoutItem> items;
    };

    using LayoutMap = QMap<AbstractElement *, LayoutItem>;
    LayoutMap m_layoutMap;
    qreal m_spacing = 0.0;

    LayoutMap layout(AbstractElement *element);

    LayoutBucket createBucket(const QList<LayoutItem> &items, const BucketType &type, const QRectF &containerRect) const;
    QRectF resizeBucket(const LayoutBucket &bucket) const;
    void mapBucketItems(LayoutBucket &bucket, LayoutMap &map) const;

    // Calculates the bounding box rectangle from items inside layoutMap
    // QRectF unifiedRect(QMap<QString, LayoutItem> layoutMap);
};
}
}
