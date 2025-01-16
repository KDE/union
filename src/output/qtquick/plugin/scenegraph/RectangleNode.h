// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <properties/BackgroundProperty.h>

#include "UnionNode.h"

class RectangleNode : public HierarchyNode
{
public:
    RectangleNode();

    Union::Properties::BackgroundProperty rectangle() const;
    void setRectangle(const Union::Properties::BackgroundProperty &newRectangle);

    void update(QQuickWindow *window) override;

private:
    void buildNodeHierarchy() override;

    Union::Properties::BackgroundProperty m_rectangle;
};
