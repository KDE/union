/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <properties/ImageProperty.h>

#include "UnionNode.h"

class AreaNode : public UnionNode
{
public:
    AreaNode();

    QColor color;
    std::optional<Union::Properties::ImageProperty> image;

    void update(QQuickWindow *window) override;
};
