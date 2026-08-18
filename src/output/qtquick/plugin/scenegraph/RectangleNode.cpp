// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "RectangleNode.h"

#include "UniformDataStream.h"

using namespace Union;
using namespace Union::Properties;
using namespace Qt::StringLiterals;

RectangleNode::RectangleNode()
{
}

void RectangleNode::setColor(const QColor &color)
{
    if (color == m_color) {
        return;
    }

    m_color = color;
    m_changed = true;
}

void RectangleNode::setRadius(const QVector4D &radius)
{
    if (radius == m_radius) {
        return;
    }

    m_radius = radius;
    m_changed = true;
}

void RectangleNode::setImage(Union::Properties::ImagePropertyGroup *image)
{
    if (image == m_image) {
        return;
    }

    m_image = image;
    m_changed = true;
}

void RectangleNode::setWindow(QQuickWindow *window)
{
    if (window == m_window) {
        return;
    }

    m_window = window;
    m_changed = true;
}

void RectangleNode::update()
{
    if (!m_changed) {
        return;
    }

    auto shaderName = u"rectangle"_s;

    auto maskColor = QColor(Qt::GlobalColor::transparent);
    if (m_image && !m_image->isEmpty()) {
        auto source = m_image->source();
        if (source.has_value()) {
            if (m_image->flags().has_value()) {
                if (m_image->flags().value().testFlag(ImageFlag::Mask)) {
                    shaderName += u"-mask"_s;
                } else if (m_image->flags().value().testFlag(ImageFlag::InvertedMask)) {
                    shaderName += u"-invertedmask"_s;
                } else {
                    shaderName += u"-texture"_s;
                }
                maskColor = m_image->maskColor().value_or(Color{}).toQColor();
            } else {
                shaderName += u"-texture"_s;
            }
        }
    }

    setShader(shaderName);
    setUniformBufferComponents(32);
    setUvChannels(2);

    auto r = rect();

    auto aspect = r.width() > r.height() ? QVector2D{float(r.width() / r.height()), 1.0} : QVector2D{1.0, float(r.height() / r.width())};
    auto minDimension = float(std::min(r.width(), r.height()));

    UniformDataStream stream(uniformData());
    stream << UniformDataStream::Placeholder::ModelViewProjectionMatrix // matrix
           << UniformDataStream::Placeholder::Opacity // opacity
           << aspect // aspect
           << ShaderNode::toPremultiplied(m_color) // color
           << ShaderNode::toPremultiplied(maskColor) // maskColor
           << m_radius / minDimension; // radius

    if (m_image && m_image->source()) {
        setTexture(1, 1, m_image->source().value(), m_window, r.size(), TextureFlag::TextureCanUseAtlas);
    }

    markDirty(QSGNode::DirtyMaterial);

    ShaderNode::update();
}
