// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <properties/ShadowProperty.h>

#include "ShaderNode.h"

/*!
 * A customized ShaderNode that renders a rectangle shadow.
 *
 * This will render a rectangle shadow of a rectangle with a given size.
 */
class RectangleShadowNode : public ShaderNode
{
public:
    RectangleShadowNode();
    /*!
     * Set the item rect for this shadow to \p newItemRect.
     *
     * The item rect determines the size of the rectangle that is casting a
     * shadow.
     */
    void setItemRect(const QRectF &newItemRect);
    /*!
     * Set the corner radii to \p radius.
     */
    void setRadius(const QVector4D &radius);
    /*!
     * Set the blur size to \p blur.
     *
     * The blur size is the area where the shadow is blurred from 100% opaque to
     * 0% opaque.
     */
    void setBlur(float blur);
    /*!
     * Set the spread size to \p spread.
     *
     * The spread size is a value that increases the shadow size from the item
     * size.
     */
    void setSpread(float spread);
    /*!
     * Set the offset to \p offset.
     *
     * The offset is a vector that determines how much the shadow is shifted
     * relative to the rectangle it is a shadow of.
     */
    void setOffset(const QVector2D &offset);
    /*!
     * Set the color to \p color.
     */
    void setColor(const QColor &color);
    /*!
     * Overrridden from ShaderNode::update()
     */
    void update() override;

private:
    bool m_changed = false;
    QRectF m_itemRect;
    QVector4D m_radius;
    float m_blur = 0.0;
    float m_spread = 0.0;
    QVector2D m_offset;
    QColor m_color;
};
