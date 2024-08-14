/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <properties/ShadowProperty.h>

#include "UnionNode.h"

class ShadowNode : public HierarchyNode
{
public:
    ShadowNode();

    Union::Properties::ShadowProperty shadow() const;
    void setShadow(const Union::Properties::ShadowProperty &newShadow);

    void update(QQuickWindow *window) override;

private:
    void buildNodeHierarchy() override;

    Union::Properties::ShadowProperty m_shadow;
};
