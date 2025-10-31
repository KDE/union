// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "ShaderNode.h"

#include <QSGTextureProvider>
#include <QVariant>

#include "ShaderMaterial.h"
#include "TextureCache.h"

struct VertexLayout {
    using RectPropertyFunction = qreal (QRectF::*)() const;
    using DataProperty = QVector4D(ShaderNode::DataChannel::*);

    RectPropertyFunction x;
    RectPropertyFunction y;
    DataProperty data;
};

static const std::array<VertexLayout, 4> Vertices = {
    VertexLayout{.x = &QRectF::left, .y = &QRectF::top, .data = &ShaderNode::DataChannel::topLeft},
    VertexLayout{.x = &QRectF::left, .y = &QRectF::bottom, .data = &ShaderNode::DataChannel::bottomLeft},
    VertexLayout{.x = &QRectF::right, .y = &QRectF::top, .data = &ShaderNode::DataChannel::topRight},
    VertexLayout{.x = &QRectF::right, .y = &QRectF::bottom, .data = &ShaderNode::DataChannel::bottomRight},
};

ShaderNode::ShaderNode()
    : m_rect(QRectF{0.0, 0.0, 1.0, 1.0})
    , m_uvs(16, QRectF{0.0, 0.0, 1.0, 1.0})
{
    setFlags(QSGNode::OwnsGeometry | QSGNode::OwnsMaterial | QSGNode::UsePreprocess);
}

ShaderNode::~ShaderNode() noexcept
{
    for (auto texture : std::as_const(m_textures)) {
        if (texture.provider) {
            texture.provider->disconnect(texture.providerConnection);
        }
    }

    setGeometry(nullptr);
    delete[] m_attributeSet->attributes;
    delete m_attributeSet;
}

void ShaderNode::preprocess()
{
    for (const auto &info : std::as_const(m_textures)) {
        if (info.provider) {
            preprocessTexture(info);
        }
    }
}

QRectF ShaderNode::rect() const
{
    return m_rect;
}

void ShaderNode::setRect(const QRectF &newRect)
{
    if (newRect == m_rect) {
        return;
    }

    m_rect = newRect;
    m_geometryUpdateNeeded = true;
}

QRectF ShaderNode::uvs(Channel channel) const
{
    Q_ASSERT(channel < m_textureChannels);
    return m_uvs[channel];
}

void ShaderNode::setUVs(Channel channel, const QRectF &newUvs)
{
    Q_ASSERT(channel < m_textureChannels);

    if (newUvs == m_uvs[channel]) {
        return;
    }

    m_uvs[channel] = newUvs;
    m_geometryUpdateNeeded = true;
}

QSGMaterialType *ShaderNode::materialVariant() const
{
    return m_materialVariant;
}

void ShaderNode::setMaterialVariant(QSGMaterialType *variant)
{
    if (variant == m_materialVariant) {
        return;
    }

    m_materialVariant = variant;

    auto newMaterial = createMaterialVariant(m_materialVariant);
    if (newMaterial) {
        m_shaderMaterial = dynamic_cast<ShaderMaterial *>(newMaterial);
        setMaterial(newMaterial);
        markDirty(QSGNode::DirtyMaterial);
    }
}

void ShaderNode::setShader(const QString &shader)
{
    setMaterialVariant(ShaderMaterial::typeForName(shader));
}

void ShaderNode::setUniformBufferSize(qsizetype size)
{
    if (!m_shaderMaterial) {
        return;
    }

    m_shaderMaterial->setUniformBufferSize(size);
}

std::span<char> ShaderNode::uniformData()
{
    if (!m_shaderMaterial) {
        return std::span<char>{};
    }

    return m_shaderMaterial->uniformData();
}

void ShaderNode::setTextureChannels(unsigned char count)
{
    if (count == m_textureChannels) {
        return;
    }

    m_textureChannels = std::clamp(count, uint8_t(1), uint8_t(16));

    if (geometry()) {
        setGeometry(nullptr);
        delete[] m_attributeSet->attributes;
        delete m_attributeSet;
    }

    while (m_textures.size() > count) {
        m_textures.removeLast();
    }

    m_geometryUpdateNeeded = true;
}

void ShaderNode::setTexture(Channel channel, const QImage &image, QQuickWindow *window, QQuickWindow::CreateTextureOptions options)
{
    if (!m_shaderMaterial) {
        return;
    }

    auto texture = TextureCache::loadTexture(window, image, options);
    if (!texture) {
        return;
    }

    auto info = TextureInfo{
        .channel = channel,
        .options = options,
        .texture = texture,
        .provider = nullptr,
        .providerConnection = {},
    };

    auto itr = std::find_if(m_textures.begin(), m_textures.end(), [channel](auto info) {
        return info.channel == channel;
    });
    if (itr != m_textures.end()) {
        *itr = info;
    } else {
        m_textures.append(info);
    }

    setUVs(channel, texture->normalizedTextureSubRect());

    m_shaderMaterial->setTexture(channel + 1, texture.get());
    markDirty(QSGNode::DirtyMaterial);
}

void ShaderNode::setTexture(Channel channel, QSGTextureProvider *provider, QQuickWindow::CreateTextureOptions options)
{
    if (!m_shaderMaterial) {
        return;
    }

    auto connection = QObject::connect(provider, &QSGTextureProvider::textureChanged, [this]() {
        markDirty(QSGNode::DirtyMaterial);
    });

    auto info = TextureInfo{
        .channel = channel,
        .options = options,
        .texture = nullptr,
        .provider = provider,
        .providerConnection = connection,
    };

    auto itr = std::find_if(m_textures.begin(), m_textures.end(), [channel](auto info) {
        return info.channel == channel;
    });
    if (itr != m_textures.end()) {
        if (itr->provider) {
            itr->provider->disconnect(itr->providerConnection);
        }
        *itr = info;
    } else {
        m_textures.append(info);
    }
}

void ShaderNode::setExtraDataChannels(unsigned char count)
{
    if (count == m_extraChannels) {
        return;
    }

    m_extraChannels = count;
    m_extraChannelData.resize(count);

    if (geometry()) {
        setGeometry(nullptr);
        delete[] m_attributeSet->attributes;
        delete m_attributeSet;
    }

    m_geometryUpdateNeeded = true;
}

void ShaderNode::update()
{
    if (m_geometryUpdateNeeded) {
        const auto attributeCount = 1 + m_textureChannels + m_extraChannels;

        if (!geometry()) {
            QSGGeometry::Attribute *attributes = new QSGGeometry::Attribute[attributeCount];
            attributes[0] = QSGGeometry::Attribute::createWithAttributeType(0, 2, QSGGeometry::FloatType, QSGGeometry::PositionAttribute);

            int start = 1;

            for (int i = 0; i < m_textureChannels; ++i) {
                attributes[i + start] = QSGGeometry::Attribute::createWithAttributeType(i + start, 2, QSGGeometry::FloatType, QSGGeometry::TexCoordAttribute);
            }

            start += m_textureChannels;

            for (int i = 0; i < m_extraChannels; ++i) {
                attributes[i + start] = QSGGeometry::Attribute::createWithAttributeType(i + start, 4, QSGGeometry::FloatType, QSGGeometry::UnknownAttribute);
            }

            int stride = sizeof(float) * 2 * (m_textureChannels + 1) + sizeof(float) * 4 * m_extraChannels;
            m_attributeSet = new QSGGeometry::AttributeSet{.count = attributeCount, .stride = stride, .attributes = attributes};

            setGeometry(new QSGGeometry{*m_attributeSet, m_vertexCount});
        }

        auto g = geometry();

        if (g->vertexCount() != m_vertexCount || g->indexCount() != m_indexCount) {
            g->allocate(m_vertexCount, m_indexCount);
            markDirty(QSGNode::DirtyGeometry);
        }

        updateGeometry(g);
        m_geometryUpdateNeeded = false;
    }
}

QSGMaterial *ShaderNode::createMaterialVariant(QSGMaterialType *variant)
{
    return new ShaderMaterial(variant);
}

void ShaderNode::setVertexCount(int count)
{
    if (count == m_vertexCount) {
        return;
    }

    m_vertexCount = count;
    m_geometryUpdateNeeded = true;
}

void ShaderNode::setIndexCount(int count)
{
    if (count == m_indexCount) {
        return;
    }

    m_indexCount = count;
    m_geometryUpdateNeeded = true;
}

void ShaderNode::requestGeometryUpdate()
{
    m_geometryUpdateNeeded = true;
}

void ShaderNode::updateGeometry(QSGGeometry *geometry)
{
    auto vertexData = static_cast<float *>(geometry->vertexData());

    for (auto layout : Vertices) {
        *vertexData++ = (m_rect.*layout.x)();
        *vertexData++ = (m_rect.*layout.y)();

        for (int channel = 0; channel < m_textureChannels; ++channel) {
            auto uv = uvs(channel);
            *vertexData++ = (uv.*layout.x)();
            *vertexData++ = (uv.*layout.y)();
        }

        for (int channel = 0; channel < m_extraChannels; ++channel) {
            auto data = m_extraChannelData[channel];
            *vertexData++ = (data.*layout.data).x();
            *vertexData++ = (data.*layout.data).y();
            *vertexData++ = (data.*layout.data).z();
            *vertexData++ = (data.*layout.data).w();
        }
    }

    markDirty(QSGNode::DirtyGeometry);
}

void ShaderNode::preprocessTexture(const TextureInfo &info)
{
    auto provider = info.provider;
    if (!provider || !m_shaderMaterial) {
        return;
    }

    if (provider->texture()->isAtlasTexture() && !info.options.testFlag(QQuickWindow::TextureCanUseAtlas)) {
        m_shaderMaterial->setTexture(info.channel + 1, provider->texture()->removedFromAtlas());
    } else {
        m_shaderMaterial->setTexture(info.channel + 1, provider->texture());
    }
    if (QSGDynamicTexture *dynamic_texture = qobject_cast<QSGDynamicTexture *>(provider->texture())) {
        dynamic_texture->updateTexture();
    }
}
