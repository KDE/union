// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QQuickItem>
#include <QQuickRenderControl>
#include <QQuickWindow>

inline qreal devicePixelRatioFor(QQuickItem *item)
{
    Q_ASSERT(item);
    Q_ASSERT(item->window());

    auto renderWindow = QQuickRenderControl::renderWindowFor(item->window());
    if (!renderWindow) {
        renderWindow = item->window();
    }

    return renderWindow->devicePixelRatio();
}

inline QPointF roundPoint(const QPointF &unrounded, float factor)
{
    return QPointF{std::round(unrounded.x() * factor) / factor, std::round(unrounded.y() * factor) / factor};
}

inline QPointF alignPoint(const QPointF &unaligned, QQuickItem *item)
{
    auto dpr = devicePixelRatioFor(item);
    auto scenePoint = item->mapToScene(unaligned);
    return item->mapFromScene(roundPoint(scenePoint, dpr));
}

inline QRectF alignRect(const QRectF &unaligned, QQuickItem *item)
{
    auto dpr = devicePixelRatioFor(item);
    auto sceneRect = item->mapRectToScene(unaligned);

    auto alignedTopLeft = roundPoint(sceneRect.topLeft(), dpr);
    auto alignedBottomRight = roundPoint(QPointF{alignedTopLeft.x() + sceneRect.width(), alignedTopLeft.y() + sceneRect.height()}, dpr);

    return item->mapRectFromScene(QRectF{alignedTopLeft, alignedBottomRight});
}
