/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <Definition.h>

#include "UnionNode.h"

class CornerNode : public UnionNode
{
public:
    CornerNode();

    Union::CornerDefinition corner;
    QRectF rect;
    QPointF offset;
    QColor color = Qt::transparent;

    void update(QQuickWindow *window) override;
};
