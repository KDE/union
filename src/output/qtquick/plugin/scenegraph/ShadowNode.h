// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <properties/ShadowPropertyGroup.h>

#include "UnionNode.h"

class ShadowNode : public HierarchyNode
{
public:
    ShadowNode();

    Union::Properties::ShadowPropertyGroup shadow() const;
    void setShadow(const Union::Properties::ShadowPropertyGroup &newShadow);

    void update(QQuickWindow *window) override;

private:
    void buildNodeHierarchy() override;

    Union::Properties::ShadowPropertyGroup m_shadow;
};
