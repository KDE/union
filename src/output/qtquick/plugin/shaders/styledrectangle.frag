// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#version 440

#extension GL_GOOGLE_include_directive: enable
#include "sdf.glsl"
// See sdf.glsl for the SDF related functions.

// This shader renders a rectangle with rounded corners and potentially a border
// and outline, with different sizes for both border and outline. It can also
// render a texture on top.

layout(std140, binding = 0) uniform buf {
    highp mat4 matrix; // 16 components
    mediump float opacity; // 17 components + 1 padding

    mediump vec2 aspect; // 20 components

    mediump vec4 borderWidth; // 24 components
    mediump vec4 outlineWidth; // 28 components
    mediump vec4 radius; // 32 components
} ubuf;

layout(location = 0) in highp vec2 uv0;
layout(location = 1) in mediump vec4 border_color;
layout(location = 2) in mediump vec4 outline_color;

layout(location = 0) out lowp vec4 out_color;

highp vec4 adjusted_rect(highp vec4 rect, highp vec4 adjustment)
{
    return vec4(rect.xy - adjustment.xy + adjustment.zw, rect.zw - (adjustment.xy + adjustment.zw));
}

highp vec4 adjusted_radius(highp vec4 radius, highp vec4 adjustment)
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

    highp vec4 rect = vec4(uv0, ubuf.aspect);
    highp vec4 corner_radius = clamped_radius;

#ifdef ENABLE_OUTLINE
    corner_radius = adjusted_radius(corner_radius, -ubuf.outlineWidth);
    col = sdf_render(sdf_rounded_rectangle(rect.xy, rect.zw, corner_radius), col, outline_color);

    rect = adjusted_rect(rect, ubuf.outlineWidth);
    corner_radius = clamped_radius;
#endif

#ifdef ENABLE_BORDER
    col = sdf_render(sdf_rounded_rectangle(rect.xy, rect.zw, corner_radius), col, border_color);
    rect = adjusted_rect(rect, ubuf.borderWidth);

    // Adjust corner radius for the amount the border makes the inner rectangle
    // smaller. Add a correction factor based on the scale of what we're
    // rendering, otherwise the corners end up being drawn slightly too small.
    corner_radius = adjusted_radius(corner_radius, ubuf.borderWidth + fwidth(uv0.x));
#endif
    // Finally, cut out the inner rectangle so we only have the border remaining.
    highp float sdf = sdf_rounded_rectangle(rect.xy, rect.zw, corner_radius);
    col = sdf_render(sdf, col, vec4(0.0, 0.0, 0.0, 0.0));

    out_color = col * ubuf.opacity;
}
