// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#version 440

#extension GL_GOOGLE_include_directive: enable
#include "sdf.glsl"
// See sdf.glsl for the SDF related functions.

// This shader renders a rectangle with rounded corners and potentially a border
// and outline, with different sizes for both border and outline. It can also
// render a texture on top.

#include "uniforms.glsl"

#ifdef ENABLE_TEXTURE
layout(binding = 1) uniform sampler2D textureSource;
#endif

layout(location = 0) in lowp vec2 uv;
layout(location = 1) in mediump vec4 border_color;
layout(location = 2) in mediump vec4 outline_color;

layout(location = 0) out lowp vec4 out_color;

mediump vec4 adjusted_rect(mediump vec4 rect, mediump vec4 adjustment)
{
    return vec4(rect.xy - adjustment.xy + adjustment.zw, rect.zw - (adjustment.xy + adjustment.zw));
}

mediump vec4 adjusted_radius(mediump vec4 radius, mediump vec4 adjustment)
{
    return vec4(
        radius.x - min(adjustment.z, adjustment.w),
        radius.y - min(adjustment.z, adjustment.y),
        radius.z - min(adjustment.x, adjustment.w),
        radius.w - min(adjustment.x, adjustment.y)
    );
}

void main()
{
    highp vec4 clamped_radius = clamp(ubuf.radius * 2.0, 0.0, 1.0);

    mediump vec4 col = vec4(0.0);

    mediump vec4 rect = vec4(uv, ubuf.aspect);

    highp vec4 corner_radius = clamped_radius;

#ifdef ENABLE_OUTLINE
    corner_radius = adjusted_radius(corner_radius, -ubuf.outlineWidth);
    col = sdf_render(sdf_rounded_rectangle(rect.xy, rect.zw, corner_radius), col, outline_color);

    rect = adjusted_rect(rect, ubuf.outlineWidth);
    corner_radius = clamped_radius;
#endif

#ifdef ENABLE_BORDER
    col = sdf_render(sdf_rounded_rectangle(rect.xy, rect.zw, corner_radius), col, border_color);
    rect = adjusted_rect(rect, ubuf.borderWidth );

    // Adjust corner radius for the amount the border makes the inner rectangle
    // smaller. Add a correction factor based on the scale of what we're
    // rendering, otherwise the corners end up being drawn slightly too small.
    corner_radius = adjusted_radius(corner_radius, ubuf.borderWidth + fwidth(uv.x));
#endif
    // Finally, render the inner rectangle.
    mediump float sdf = sdf_rounded_rectangle(rect.xy, rect.zw, corner_radius);
    col = sdf_render(sdf, col, ubuf.color);

#ifdef ENABLE_TEXTURE
    // Sample the texture, then blend it on top of the background color.
    highp vec2 texture_uv = uv / (ubuf.aspect * 2.0) + 0.5;
    mediump vec4 texture_color = texture(textureSource, texture_uv);
#ifdef ENABLE_MASK
    texture_color = vec4(ubuf.mask_color.xyz * texture_color.a, texture_color.a);
#endif
#ifdef ENABLE_INVERTEDMASK
    texture_color = vec4(ubuf.mask_color.xyz * (1 - texture_color.a), (1 - texture_color.a));
#endif
    col = sdf_render(sdf, col, texture_color, texture_color.a, sdf_default_smoothing);
#endif

    out_color = col * ubuf.opacity;
}
