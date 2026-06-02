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
    Union::Properties::BackgroundPropertyGroup *m_background;
    Union::Properties::BorderPropertyGroup *m_border;
    Union::Properties::OutlinePropertyGroup *m_outline;
    QVector4D m_radius;
    QQuickWindow *m_window = nullptr;

    void update() override;

protected:
    void updateGeometry(QSGGeometry *geometry) override;

private:
    struct Vertex {
        QVector2D position;
        QVector2D texture0;
        QVector2D texture1;
        QColor border;
        QColor outline;
    };

    void updateVertices(const QRectF &rect, const QVector4D &radii, const QVector4D &borderSize, const QVector4D &outlineSize);
    void updateBorderColors(const Union::Properties::BorderPropertyGroup *border, const QColor &center);
    void updateOutlineColors(const Union::Properties::OutlinePropertyGroup *border, const QColor &center);
    void updateColors(QColor(Vertex::*destination), const QColor &left, const QColor &right, const QColor &top, const QColor &bottom, const QColor &center);

    std::array<Vertex, 28> m_vertices;
};
