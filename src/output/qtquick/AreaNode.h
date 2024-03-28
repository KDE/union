/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <Definition.h>
// #include <Size.h>

#include "UnionNode.h"

class AreaNode : public UnionNode
{
public:
    AreaNode();

    Union::AreaDefinition area;
    QRectF rect;

    void update(QQuickWindow *window) override;
};
