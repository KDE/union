// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "OutlineBorderRectangleNode.h"

#include "UniformDataStream.h"

using namespace Union;
using namespace Union::Properties;
using namespace Qt::StringLiterals;

inline QColor mix(const QColor &first, const QColor &second, float amount = 0.5)
{
    const auto inverseAmount = 1.0 - amount;
    return QColor::fromRgbF(first.redF() * amount + second.redF() * inverseAmount,
                            first.greenF() * amount + second.greenF() * inverseAmount,
                            first.blueF() * amount + second.blueF() * inverseAmount,
                            first.alphaF() * amount + second.alphaF() * inverseAmount);
}

template<typename T>
inline QVector4D toVector4D(const T *property)
{
    QVector4D result;

    auto directionValue = [](LineProperty *line) {
        if (line) {
            if (line->style().value_or(Union::Properties::LineStyle::None) != Union::Properties::LineStyle::None) {
                return line->size().value_or(0.0);
            }
        }
        return 0.0;
    };

    result.setX(directionValue(property->left()));
    result.setZ(directionValue(property->right()));
    result.setY(directionValue(property->top()));
    result.setW(directionValue(property->bottom()));

    return result;
}

/* Index data for the following geometry:
 *
 * 0-----2------5-----9
 * |    *|     *|*    |
 * |  *  |   *  |  *  |
 * |*    | *    |    *|
 * 1----3,4----6,7----8
 * |   11,24  26,16  *|
 * |  *  |   *  |  *  |
 * |*    | *    |*    |
 * 10--12,25--27,17--18
 * |*  15,22  23,19  *|
 * |  *  |  *   |  *  |
 * |    *|*     |*    |
 * 13---14-----20----21
 *
 * This geometry is used for outline and border colors, by setting the vertex
 * colors of the outer ring of triangles. The corners are meant to be blended
 * when border colors differ. Some of the vertices are duplicated since they
 * should *not* blend.
 */
static constexpr std::array<uint16_t, 54> Indices = {
    /* clang-format off */
    // Left-Top
    0,  1,  2,
    1,  3,  2,
    // Top
    2,  4,  5,
    4,  6,  5,
    // Right-Top
    5,  7,  8,
    5,  8,  9,
    // Left
    1, 10, 11,
    10, 12, 11,
    // Left-Bottom
    10, 13, 14,
    10, 14, 15,
    // Right
    16, 17,  8,
    8, 17, 18,
    // Right-Bottom
    19, 20, 18,
    18, 20, 21,
    // Bottom
    22, 14, 23,
    14, 20, 23,
    // Center
    24, 25, 26,
    25, 27, 26,
    /* clang-format on */
};

OutlineBorderRectangleNode::OutlineBorderRectangleNode()
{
    setVertexCount(m_vertices.size());
    setIndexCount(Indices.size());

    setTextureChannels(1);
    setExtraDataChannels(2);
}

void OutlineBorderRectangleNode::update()
{
    auto shaderName = u"styledrectangle"_s;

    QVector4D borderSize;
    if (m_border) {
        borderSize = toVector4D(m_border);
        if (!borderSize.isNull()) {
            shaderName += u"-border"_s;
        }
    }

    QVector4D outlineSize;
    if (m_outline) {
        outlineSize = toVector4D(m_outline);
        if (!outlineSize.isNull()) {
            shaderName += u"-outline"_s;
        }
    }

    auto maskColor = QColor(Qt::GlobalColor::transparent);
    if (m_background && m_background->image() && !m_background->image()->isEmpty()) {
        auto imageProperties = m_background->image();
        auto image = imageProperties->imageData();
        if (image.has_value()) {
            shaderName += u"-texture"_s;
            if (imageProperties->flags().has_value()) {
                if (imageProperties->flags().value().testFlag(ImageFlag::Mask)) {
                    shaderName += u"-mask"_s;
                } else if (imageProperties->flags().value().testFlag(ImageFlag::InvertedMask)) {
                    shaderName += u"-invertedmask"_s;
                }
                maskColor = imageProperties->maskColor().value_or(Color{}).toQColor();
            }
        }
    }

    setShader(shaderName);
    setUniformBufferSize(sizeof(float) * 40);

    auto aspect = m_itemRect.width() > m_itemRect.height() ? QVector2D{float(m_itemRect.width() / m_itemRect.height()), 1.0}
                                                           : QVector2D{1.0, float(m_itemRect.height() / m_itemRect.width())};
    auto minDimension = float(std::min(m_itemRect.width(), m_itemRect.height()));

    auto backgroundColor = (m_background ? m_background->color().value_or(Color{}) : Color{}).toQColor();

    if (m_border && !borderSize.isNull()) {
        updateBorderColors(m_border, backgroundColor);
    }

    if (m_outline && !outlineSize.isNull()) {
        updateOutlineColors(m_outline, backgroundColor);
    }

    updateVertices(m_itemRect, m_radius, borderSize, outlineSize);
    requestGeometryUpdate();

    UniformDataStream stream(uniformData());
    stream.skipMatrixOpacity();
    stream << aspect // aspect
           << borderSize / minDimension // border_width
           << outlineSize / minDimension // outline_width
           << m_radius / minDimension // radius
           << ShaderNode::toPremultiplied(backgroundColor) // color
           << ShaderNode::toPremultiplied(maskColor); // mask-color

    if (m_background && m_background->image() && !m_background->image()->isEmpty()) {
        setTexture(0, m_background->image()->imageData().value(), m_window);
    }

    markDirty(QSGNode::DirtyMaterial);

    ShaderNode::update();
}

void OutlineBorderRectangleNode::updateGeometry(QSGGeometry *geometry)
{
    geometry->setDrawingMode(QSGGeometry::DrawingMode::DrawTriangles);

    auto vertexData = static_cast<float *>(geometry->vertexData());
    const auto vertexCount = geometry->vertexCount();

    for (int i = 0; i < vertexCount; ++i) {
        auto vertex = m_vertices[i];

        *vertexData++ = vertex.position.x();
        *vertexData++ = vertex.position.y();

        *vertexData++ = vertex.texture.x();
        *vertexData++ = vertex.texture.y();

        *vertexData++ = vertex.border.redF();
        *vertexData++ = vertex.border.greenF();
        *vertexData++ = vertex.border.blueF();
        *vertexData++ = vertex.border.alphaF();

        *vertexData++ = vertex.outline.redF();
        *vertexData++ = vertex.outline.greenF();
        *vertexData++ = vertex.outline.blueF();
        *vertexData++ = vertex.outline.alphaF();
    }

    memcpy(geometry->indexData(), Indices.data(), geometry->indexCount() * sizeof(uint16_t));

    // TODO: Be more intelligent about change detection
    geometry->markVertexDataDirty();
    geometry->markIndexDataDirty();
    markDirty(QSGNode::DirtyGeometry);
}

void OutlineBorderRectangleNode::updateVertices(const QRectF &rect, const QVector4D &radii, const QVector4D &borderSize, const QVector4D &outlineSize)
{
    const float width = rect.width();
    const float height = rect.height();

    const float left = rect.x() - outlineSize.x();
    const float right = rect.x() + width + outlineSize.z();
    const float top = rect.y() - outlineSize.y();
    const float bottom = rect.y() + height + outlineSize.w();

    // Shader corner radius order is bottom right, top right, bottom left, top left.

    const float leftTopWidth = std::max(outlineSize.x() + borderSize.x(), std::min(radii.w(), width / 2));
    const float leftBottomWidth = std::max(outlineSize.x() + borderSize.x(), std::min(radii.z(), width / 2));
    const float rightTopWidth = std::max(outlineSize.z() + borderSize.z(), std::min(radii.y(), width / 2));
    const float rightBottomWidth = std::max(outlineSize.z() + borderSize.z(), std::min(radii.x(), width / 2));

    const float leftTopHeight = std::max(outlineSize.y() + borderSize.y(), std::min(radii.w(), height / 2));
    const float leftBottomHeight = std::max(outlineSize.w() + borderSize.w(), std::min(radii.z(), height / 2));
    const float rightTopHeight = std::max(outlineSize.y() + borderSize.y(), std::min(radii.y(), height / 2));
    const float rightBottomHeight = std::max(outlineSize.w() + borderSize.w(), std::min(radii.x(), height / 2));

    float leftTopU = leftTopWidth / rect.width();
    float leftBottomU = leftBottomWidth / rect.width();
    float rightTopU = 1.0 - (rightTopWidth / rect.width());
    float rightBottomU = 1.0 - (rightBottomWidth / rect.width());
    float leftTopV = leftTopHeight / rect.height();
    float leftBottomV = 1.0 - (leftBottomHeight / rect.height());
    float rightTopV = rightTopHeight / rect.height();
    float rightBottomV = 1.0 - (rightBottomHeight / rect.height());

    // Left-Top
    m_vertices[0].position = QVector2D(left, top);
    m_vertices[1].position = QVector2D(left, top + leftTopHeight);
    m_vertices[2].position = QVector2D(left + leftTopWidth, top);
    m_vertices[3].position = QVector2D(left + leftTopWidth, top + leftTopHeight);

    m_vertices[0].texture = QVector2D(0.0, 0.0);
    m_vertices[1].texture = QVector2D(0.0, leftTopV);
    m_vertices[2].texture = QVector2D(leftTopU, 0.0);
    m_vertices[3].texture = QVector2D(leftTopU, leftTopV);

    // Top
    m_vertices[4].position = QVector2D(left + leftTopWidth, top + leftTopHeight);
    m_vertices[5].position = QVector2D(right - rightTopWidth, top);
    m_vertices[6].position = QVector2D(right - rightTopWidth, top + rightTopHeight);

    m_vertices[4].texture = QVector2D(leftTopU, leftTopV);
    m_vertices[5].texture = QVector2D(rightTopU, 0.0);
    m_vertices[6].texture = QVector2D(rightTopU, rightTopV);

    // Right-Top
    m_vertices[7].position = QVector2D(right - rightTopWidth, top + rightTopHeight);
    m_vertices[8].position = QVector2D(right, top + rightTopHeight);
    m_vertices[9].position = QVector2D(right, top);

    m_vertices[7].texture = QVector2D(rightTopU, rightTopV);
    m_vertices[8].texture = QVector2D(1.0, rightTopV);
    m_vertices[9].texture = QVector2D(1.0, 0.0);

    // Left
    m_vertices[10].position = QVector2D(left, bottom - leftBottomHeight);
    m_vertices[11].position = QVector2D(left + leftTopWidth, top + leftTopHeight);
    m_vertices[12].position = QVector2D(left + leftBottomWidth, bottom - leftBottomHeight);

    m_vertices[10].texture = QVector2D(0.0, leftBottomV);
    m_vertices[11].texture = QVector2D(leftTopU, leftTopV);
    m_vertices[12].texture = QVector2D(leftBottomU, leftBottomV);

    // Left-Bottom
    m_vertices[13].position = QVector2D(left, bottom);
    m_vertices[14].position = QVector2D(left + leftBottomWidth, bottom);
    m_vertices[15].position = QVector2D(left + leftBottomWidth, bottom - leftBottomHeight);

    m_vertices[13].texture = QVector2D(0.0, 1.0);
    m_vertices[14].texture = QVector2D(leftBottomU, 1.0);
    m_vertices[15].texture = QVector2D(leftBottomU, leftBottomV);

    // Right
    m_vertices[16].position = QVector2D(right - rightTopWidth, top + rightTopHeight);
    m_vertices[17].position = QVector2D(right - rightBottomWidth, bottom - rightBottomHeight);
    m_vertices[18].position = QVector2D(right, bottom - rightBottomHeight);

    m_vertices[16].texture = QVector2D(rightTopU, rightTopV);
    m_vertices[17].texture = QVector2D(rightBottomU, rightBottomV);
    m_vertices[18].texture = QVector2D(1.0, rightBottomV);

    // Right-Bottom
    m_vertices[19].position = QVector2D(right - rightBottomWidth, bottom - rightBottomHeight);
    m_vertices[20].position = QVector2D(right - rightBottomWidth, bottom);
    m_vertices[21].position = QVector2D(right, bottom);

    m_vertices[19].texture = QVector2D(rightBottomU, rightBottomV);
    m_vertices[20].texture = QVector2D(rightBottomU, 1.0);
    m_vertices[21].texture = QVector2D(1.0, 1.0);

    // Bottom
    m_vertices[22].position = QVector2D(left + leftBottomWidth, bottom - leftBottomHeight);
    m_vertices[23].position = QVector2D(right - rightBottomWidth, bottom - rightBottomHeight);

    m_vertices[22].texture = QVector2D(leftBottomU, leftBottomV);
    m_vertices[23].texture = QVector2D(rightBottomU, rightBottomV);

    // Center
    m_vertices[24].position = QVector2D(left + leftTopWidth, top + leftTopHeight);
    m_vertices[25].position = QVector2D(left + leftBottomWidth, bottom - leftBottomHeight);
    m_vertices[26].position = QVector2D(right - rightTopWidth, top + rightTopHeight);
    m_vertices[27].position = QVector2D(right - rightBottomWidth, bottom - rightBottomHeight);

    m_vertices[24].texture = QVector2D(leftTopU, leftTopV);
    m_vertices[25].texture = QVector2D(leftBottomU, leftBottomV);
    m_vertices[26].texture = QVector2D(rightTopU, rightTopV);
    m_vertices[27].texture = QVector2D(rightBottomU, rightBottomV);
}

void OutlineBorderRectangleNode::updateBorderColors(const Union::Properties::BorderProperty *border, const QColor &center)
{
    auto borderColor = [](auto border) {
        return (border ? border->color().value_or(Union::Color{}) : Union::Color{}).toQColor();
    };

    auto left = ShaderNode::toPremultiplied(borderColor(border->left()));
    auto right = ShaderNode::toPremultiplied(borderColor(border->right()));
    auto top = ShaderNode::toPremultiplied(borderColor(border->top()));
    auto bottom = ShaderNode::toPremultiplied(borderColor(border->bottom()));

    updateColors(&Vertex::border, left, right, top, bottom, center);
}

void OutlineBorderRectangleNode::updateOutlineColors(const Union::Properties::OutlineProperty *outline, const QColor &center)
{
    auto outlineColor = [](auto outline) {
        return (outline ? outline->color().value_or(Union::Color{}) : Union::Color{}).toQColor();
    };

    auto left = ShaderNode::toPremultiplied(outlineColor(outline->left()));
    auto right = ShaderNode::toPremultiplied(outlineColor(outline->right()));
    auto top = ShaderNode::toPremultiplied(outlineColor(outline->top()));
    auto bottom = ShaderNode::toPremultiplied(outlineColor(outline->bottom()));

    updateColors(&Vertex::outline, left, right, top, bottom, center);
}

void OutlineBorderRectangleNode::updateColors(QColor(Vertex::*destination),
                                              const QColor &left,
                                              const QColor &right,
                                              const QColor &top,
                                              const QColor &bottom,
                                              const QColor &center)
{
    // Left-Top
    m_vertices[0].*destination = mix(left, top);
    m_vertices[1].*destination = left;
    m_vertices[2].*destination = top;
    m_vertices[3].*destination = mix(left, top);

    // Top
    m_vertices[4].*destination = top;
    m_vertices[5].*destination = top;
    m_vertices[6].*destination = top;

    // Right-Top
    m_vertices[7].*destination = mix(right, top);
    m_vertices[8].*destination = right;
    m_vertices[9].*destination = mix(right, top);

    // Left
    m_vertices[10].*destination = left;
    m_vertices[11].*destination = left;
    m_vertices[12].*destination = left;

    // Left-Bottom
    m_vertices[13].*destination = mix(left, bottom);
    m_vertices[14].*destination = bottom;
    m_vertices[15].*destination = mix(left, bottom);

    // Right
    m_vertices[16].*destination = right;
    m_vertices[17].*destination = right;
    m_vertices[18].*destination = right;

    // Right-Bottom
    m_vertices[19].*destination = mix(right, bottom);
    m_vertices[20].*destination = bottom;
    m_vertices[21].*destination = mix(right, bottom);

    // Bottom
    m_vertices[22].*destination = bottom;
    m_vertices[23].*destination = bottom;

    // Center
    m_vertices[24].*destination = center;
    m_vertices[25].*destination = center;
    m_vertices[26].*destination = center;
    m_vertices[27].*destination = center;
}
