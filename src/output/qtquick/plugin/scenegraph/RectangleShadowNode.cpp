// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "RectangleShadowNode.h"

#include "UniformDataStream.h"

using namespace Union;
using namespace Union::Properties;
using namespace Qt::StringLiterals;

RectangleShadowNode::RectangleShadowNode()
{
    setShader(u"rectangleshadow"_s);

    setUniformBufferSize(sizeof(float) * 32);
    setUvChannels(1);
}

void RectangleShadowNode::setItemRect(const QRectF &newItemRect)
{
    if (newItemRect == m_itemRect) {
        return;
    }

    m_itemRect = newItemRect;
    m_changed = true;
}

void RectangleShadowNode::setRadius(const QVector4D &radius)
{
    if (radius == m_radius) {
        return;
    }

    m_radius = radius;
    m_changed = true;
}

void RectangleShadowNode::setBlur(float blur)
{
    if (qFuzzyCompare(blur, m_blur)) {
        return;
    }

    m_blur = blur;
    m_changed = true;
}

void RectangleShadowNode::setSpread(float spread)
{
    if (qFuzzyCompare(spread, m_spread)) {
        return;
    }

    m_spread = spread;
    m_changed = true;
}

void RectangleShadowNode::setOffset(const QVector2D &offset)
{
    if (offset == m_offset) {
        return;
    }

    m_offset = offset;
    m_changed = true;
}

void RectangleShadowNode::setColor(const QColor &color)
{
    if (color == m_color) {
        return;
    }

    m_color = color;
    m_changed = true;
}

void RectangleShadowNode::update()
{
    if (!m_changed) {
        return;
    }

    m_changed = false;

    auto size = m_spread + m_blur;

    auto r = m_itemRect.adjusted(-size, -size, size, size);
    r.translate(m_offset.x(), m_offset.y());
    setRect(r);

    auto aspect = r.width() > r.height() ? QVector2D{float(r.width() / r.height()), 1.0} : QVector2D{1.0, float(r.height() / r.width())};
    auto minDimension = std::min(r.width(), r.height());

    UniformDataStream stream(uniformData());
    stream.skipMatrixOpacity();
    stream << aspect // aspect
           << float(m_spread / minDimension) // spread
           << float(m_blur / minDimension) // blur
           << m_radius / minDimension // radius
           << ShaderNode::toPremultiplied(m_color); // color

    markDirty(QSGNode::DirtyMaterial);

    ShaderNode::update();
}
