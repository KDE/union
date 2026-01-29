// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <filesystem>
#include <span>

#include <QSGGeometryNode>
#include <QSGMaterial>
#include <QSGMaterialShader>
#include <QSGTextureProvider>

class ShaderMaterial;

/*!
 * A base class for scene graph nodes that want to use a shader to render something.
 */
class ShaderNode : public QSGGeometryNode
{
public:
    using Channel = uint8_t;
    using Binding = uint8_t;

    struct TextureInfo {
        Channel channel = 0;
        Binding binding = 0;
        QQuickWindow::CreateTextureOptions options;
        std::shared_ptr<QSGTexture> texture = nullptr;
        QPointer<QSGTextureProvider> provider = nullptr;
        QMetaObject::Connection providerConnection;
    };

    struct DataChannel {
        QVector4D topLeft;
        QVector4D topRight;
        QVector4D bottomLeft;
        QVector4D bottomRight;
    };

    ShaderNode();
    ~ShaderNode() override;

    void preprocess() override;

    /*!
     * The rectangle describing the geometry of this node.
     */
    QRectF rect() const;
    void setRect(const QRectF &newRect);

    /*!
     * The UV coordinates of the geometry of this node.
     */
    QRectF uvs(Channel channel) const;
    void setUVs(Channel channel, const QRectF &newUvs);

    /*!
     * The variant of the material used for rendering.
     *
     * This will be passed to createMaterialVariant() to perform the actual
     * creation of the material.
     */
    QSGMaterialType *materialVariant() const;
    void setMaterialVariant(QSGMaterialType *variant);

    /*!
     * Set the name of the shader to use for rendering.
     *
     * By default this will create and use an instance of ShaderMaterial that
     * corresponds to the given shader.
     */
    void setShader(const QString &shader);

    /*!
     * Set the size of buffer used by the material for storing uniform values.
     *
     * The given size is in bytes. Note that you should account for all uniforms
     * in your shader's uniform buffer.
     */
    void setUniformBufferSize(qsizetype size);

    /*!
     * A writeable view of the material's uniform data buffer.
     *
     * This can be used in combination with UniformDataStream to write the
     * values of your uniforms.
     */
    std::span<char> uniformData();

    /*!
     * Set the number of UV channels.
     *
     * Each UV channel gets its own set of coordinates. By default, the UVs will
     * be set to (0, 0, 1, 1).
     */
    void setUvChannels(unsigned char count);

    /*!
     * Set the texture for a channel to an image.
     *
     * This will create a texture from \p image using \p window and the options
     * specified by \p options. It will use UV coordinates from UV channel
     * \p channel and will be assigned to texture binding \p binding of the
     * material.
     *
     * Textures created from images are cached, if an image has the same cache
     * ID as a previous call to setTexture(), no new texture will be created.
     *
     * The UV coordinates of \p channel will be updated to match the expected
     * coordinates for the texture, such that textures from a texture atlas work
     * correctly.
     */
    void setTexture(Channel channel, Binding binding, const QImage &image, QQuickWindow *window, QQuickWindow::CreateTextureOptions options = {});

    /*!
     * Set the texture for a channel to an image loaded from disk.
     *
     * This will load the image at \p path at size \p size and create a texture
     * from its image using \p window, and \p options. It will use UV
     * coordinates from UV channel \p channel and will be assigned to texture
     * binding \p binding of the material.
     *
     * Both the image and texture will be cached, so repeated calls to this
     * method with the same arguments will be significantly faster.
     *
     * The UV coordinates of \p channel will be updated to match the expected
     * coordinates for the texture, such that textures from a texture atlas work
     * correctly.
     */
    void setTexture(Channel channel,
                    Binding binding,
                    const std::filesystem::path &path,
                    QQuickWindow *window,
                    const QSizeF &size = {},
                    QQuickWindow::CreateTextureOptions options = {});

    /*!
     * Set the texture for a binding to a texture.
     *
     * This will assign \p texture to binding \p binding of the material. The
     * texture will use UV coordinates from \p channel. \p options is used to
     * determine if the texture is part of an atlas.
     *
     * The UV coordinates of \p channel will be updated to match the expected
     * coordinates for the texture, such that textures from a texture atlas work
     * correctly.
     */
    void setTexture(Channel channel, Binding binding, const std::shared_ptr<QSGTexture> &texture, QQuickWindow::CreateTextureOptions options = {});

    /*!
     * Set the texture for a channel to a texture provider.
     *
     * This will use \p provider to provide the texture for binding \p binding
     * \p options will be used whenever a new texture is created from
     * \p provider. The texture will use UV coordinates from channel \p channel.
     *
     * The UV coordinates of \p channel will be updated to match the expected
     * coordinates for the texture, such that textures from a texture atlas work
     * correctly.
     */
    void setTexture(Channel channel, Binding binding, QSGTextureProvider *provider, QQuickWindow::CreateTextureOptions options = {});

    /*!
     * Set the number of extra vertex data channels to \p count.
     *
     * Extra vertex data channels are used to store extra data in the vertices
     * of the geometry. These can be used to store colors or other values. By
     * default ShaderNode uses a single quad as geometry with no extra data
     * channels.
     */
    void setExtraDataChannels(unsigned char count);

    /*!
     * Set the data for channel \p channel to the provided values.
     *
     * \p topLeft, \p topRight, \p bottomLeft and \p bottomRight are used to set
     * the data value of the default quad's vertices to the specified value.
     */
    void setExtraDataChannelData(Channel channel, //
                                 const QVector4D &topLeft,
                                 const QVector4D &topRight,
                                 const QVector4D &bottomLeft,
                                 const QVector4D &bottomRight);

    /*!
     * Set the data for channel \p channel to the provided colors.
     *
     * \p topLeft, \p topRight, \p bottomLeft and \p bottomRight are used to set
     * the data value of the default quad's vertices to the specified color
     * value.
     */
    void setExtraDataChannelData(Channel channel, const QColor &topLeft, const QColor &topRight, const QColor &bottomLeft, const QColor &bottomRight);

    /*!
     * Update internal state based on newly-set parameters.
     *
     * This is done as an explicit step to ensure we don't modify expensive GPU
     * resources like geometry multiple times during a single update.
     */
    virtual void update();

    /*!
     * Helper function that returns a pre-multiplied version of a color.
     */
    static inline QColor toPremultiplied(const QColor &value)
    {
        return QColor::fromRgbF(value.redF() * value.alphaF(), //
                                value.greenF() * value.alphaF(),
                                value.blueF() * value.alphaF(),
                                value.alphaF());
    }

protected:
    /*!
     * Create a new instance of a certain material variant.
     *
     * This should return a new instance of the material that matches \p variant,
     * or nullptr if the specified variant cannot be handled by the current node.
     */
    virtual QSGMaterial *createMaterialVariant(QSGMaterialType *variant);
    /*!
     * Set the number of vertices used by the geometry of this node to \p count.
     *
     * By default a single quad is used, if you set this you will also want to
     * override updateGeometry() .
     */
    void setVertexCount(int count);
    /*!
     * Set the number of indices used by the geometry of this node to \p count.
     *
     * The default geometry uses no indices. If you set this you will also want
     * to override updateGeometry() to make use of the indices.
     */
    void setIndexCount(int count);
    /*!
     * Request an update of the geometry data.
     *
     * Geometry updates are only performed on request for performance reasons.
     */
    void requestGeometryUpdate();
    /*!
     * Update the geometry data of this node.
     *
     * Override this if you want to change the vertex and/or index data.
     */
    virtual void updateGeometry(QSGGeometry *geometry);

private:
    void preprocessTexture(const TextureInfo &texture);

    QRectF m_rect;
    bool m_geometryUpdateNeeded = true;

    unsigned char m_uvChannels = 1;
    QVarLengthArray<QRectF, 16> m_uvs;

    unsigned char m_extraChannels = 0;
    QVector<DataChannel> m_extraChannelData;

    int m_vertexCount = 4;
    int m_indexCount = 0;

    QSGMaterialType *m_materialVariant = nullptr;
    ShaderMaterial *m_shaderMaterial = nullptr;

    QList<TextureInfo> m_textures;

    QSGGeometry::AttributeSet *m_attributeSet = nullptr;
};
