/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <Definition.h>

#include "UnionNode.h"

class LineNode : public UnionNode
{
public:
    LineNode();

    Union::LineDefinition line;
    QRectF rect;
    Qt::Orientation orientation = Qt::Horizontal;
    Qt::Alignment alignment;

    void update(QQuickWindow *window) override;
};
