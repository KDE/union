// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <properties/ImagePropertyGroup.h>

#include "ShaderNode.h"

class RectangleNode : public ShaderNode
{
public:
    RectangleNode();

    void setColor(const QColor &color);
    void setRadius(const QVector4D &radius);
    void setImage(Union::Properties::ImagePropertyGroup *image);
    void setWindow(QQuickWindow *window);

    void update() override;

private:
    bool m_changed = true;
    QColor m_color;
    QVector4D m_radius;
    Union::Properties::ImagePropertyGroup *m_image = nullptr;
    QQuickWindow *m_window = nullptr;
};
