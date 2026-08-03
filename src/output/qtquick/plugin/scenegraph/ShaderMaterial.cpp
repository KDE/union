// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "ShaderMaterial.h"
#include "scenegraph/UniformDataStream.h"

#include <QSGTexture>
#include <QVariant>

using namespace Qt::StringLiterals;

ShaderMaterial::ShaderMaterial(const QString &name)
    : m_name(name)
{
    m_type = typeForName(name);
    setFlag(QSGMaterial::Blending, true);
}

ShaderMaterial::ShaderMaterial(QSGMaterialType *type)
    : m_type(type)
{
    m_name = nameForType(type);
    setFlag(QSGMaterial::Blending, true);
}

QString ShaderMaterial::name() const
{
    return m_name;
}

QSGMaterialShader *ShaderMaterial::createShader(QSGRendererInterface::RenderMode) const
{
    return new ShaderMaterialShader{m_name};
}

QSGMaterialType *ShaderMaterial::type() const
{
    return m_type;
}

int ShaderMaterial::compare(const QSGMaterial *other) const
{
    auto material = static_cast<const ShaderMaterial *>(other);
    if (m_uniformData == material->m_uniformData && m_textures == material->m_textures) {
        return 0;
    }

    return QSGMaterial::compare(other);
}

void ShaderMaterial::setUniformBufferSize(qsizetype size)
{
    if (size == m_uniformData.size()) {
        return;
    }

    m_uniformData = QByteArray{size, '\0'};
}

std::span<char> ShaderMaterial::uniformData()
{
    return std::span(m_uniformData.data(), m_uniformData.size());
}

QSGTexture *ShaderMaterial::texture(int binding)
{
    return m_textures.value(binding, nullptr);
}

void ShaderMaterial::setTexture(int binding, QSGTexture *texture)
{
    m_textures[binding] = texture;
}

QString ShaderMaterial::nameForType(QSGMaterialType *type)
{
    for (auto &[key, value] : s_materialTypes) {
        if (value.get() == type) {
            return key;
        }
    }
    return QString();
}

QSGMaterialType *ShaderMaterial::typeForName(const QString &name)
{
    if (!s_materialTypes.contains(name)) {
        s_materialTypes[name] = std::make_unique<QSGMaterialType>();
    }
    return s_materialTypes[name].get();
}

ShaderMaterialShader::ShaderMaterialShader(const QString &shaderName)
{
    static const auto shaderRoot = QStringLiteral(":/qt/qml/org/kde/union/impl/shaders/");

    setShaderFileName(Stage::VertexStage, shaderRoot + shaderName + u".vert.qsb");
    setShaderFileName(Stage::FragmentStage, shaderRoot + shaderName + u".frag.qsb");
}

bool ShaderMaterialShader::updateUniformData(RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial)
{
    bool changed = false;

    auto data = state.uniformData();

    if (!oldMaterial || newMaterial->compare(oldMaterial) != 0) {
        auto uniformData = static_cast<ShaderMaterial *>(newMaterial)->uniformData();
        Q_ASSERT(std::size_t(data->size()) == uniformData.size());

        data->assign(uniformData);

        auto dataBytes = data->data();
        auto intData = reinterpret_cast<int *>(dataBytes);
        for (std::size_t i = 0; i < (data->size() / sizeof(int)); ++i) {
            switch (intData[i]) {
            case UniformDataStream::ModelViewProjectionPlaceholder: {
                auto matrix = state.combinedMatrix();
                memcpy(dataBytes + i * sizeof(int), matrix.data(), sizeof(float) * 16);
                break;
            }
            case UniformDataStream::OpacityPlaceholder: {
                auto opacity = state.opacity();
                memcpy(dataBytes + i * sizeof(int), &opacity, sizeof(float));
                break;
            }
            case UniformDataStream::ViewportPlaceholder: {
                auto viewport = state.viewportRect();
                auto vec = QVector4D{float(viewport.x()), float(viewport.y()), float(viewport.width()), float(viewport.height())};
                memcpy(dataBytes + i * sizeof(int), &vec, sizeof(float) * 4);
                break;
            }
            }
        }

        changed = true;
    }

    return changed;
}

void ShaderMaterialShader::updateSampledImage(QSGMaterialShader::RenderState &state,
                                              int binding,
                                              QSGTexture **texture,
                                              QSGMaterial *newMaterial,
                                              QSGMaterial *oldMaterial)
{
    Q_UNUSED(oldMaterial);

    auto material = static_cast<ShaderMaterial *>(newMaterial);
    auto source = material->texture(binding);
    if (source) {
        source->commitTextureOperations(state.rhi(), state.resourceUpdateBatch());
        *texture = source;
    } else {
        *texture = nullptr;
    }
}
