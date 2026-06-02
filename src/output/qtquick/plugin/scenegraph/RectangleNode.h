// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <properties/StylePropertyGroup.h>

#include "UnionNode.h"

class RectangleNode : public HierarchyNode
{
public:
    RectangleNode();

    Union::Properties::StylePropertyGroup style() const;
    void setStyle(const Union::Properties::StylePropertyGroup &newStyle);

    void update(QQuickWindow *window) override;

private:
    void buildNodeHierarchy() override;

    Union::Properties::StylePropertyGroup m_style;
};
