// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <properties/BackgroundPropertyGroup.h>
#include <properties/BorderPropertyGroup.h>
#include <properties/OutlinePropertyGroup.h>

#include "ShaderNode.h"

class OutlineBorderRectangleNode : public ShaderNode
{
public:
    OutlineBorderRectangleNode();

    QRectF m_itemRect;
    Union::Properties::BorderPropertyGroup *m_border = nullptr;
    Union::Properties::OutlinePropertyGroup *m_outline = nullptr;
    QVector4D m_radius;
    QQuickWindow *m_window = nullptr;

    void update() override;

protected:
    void updateGeometry(QSGGeometry *geometry) override;

private:
    struct Vertex {
        QVector2D position;
        QVector2D texture0;
        QVector4D border;
        QVector4D outline;
    };

    void updateVertices(const QRectF &rect, QVector4D radii, QVector4D borderSize, QVector4D outlineSize);
    void updateBorderColors(const Union::Properties::BorderPropertyGroup *border);
    void updateOutlineColors(const Union::Properties::OutlinePropertyGroup *border);
    void updateColors(QVector4D(Vertex::*destination), QVector4D left, QVector4D right, QVector4D top, QVector4D bottom);

    std::array<Vertex, 28> m_vertices;
};
