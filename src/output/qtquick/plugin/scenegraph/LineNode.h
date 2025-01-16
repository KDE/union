// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <properties/LineProperty.h>

#include "UnionNode.h"

class LineNode : public UnionNode
{
public:
    LineNode();

    Union::Properties::LineProperty line;

    void update(QQuickWindow *window) override;
};
