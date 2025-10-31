// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#version 440

#extension GL_GOOGLE_include_directive: enable
#include "sdf.glsl"
// See sdf.glsl for the SDF related functions.

// This shader renders a rectangle shadow with rounded corners.

layout(std140, binding = 0) uniform buf {
    highp mat4 matrix; // 16 components
    mediump float opacity; // 17 components + 1 padding

    mediump vec2 aspect; // 20 components

    mediump float spread; // 21 components
    mediump float blur; // 22 components + 2 padding
    mediump vec4 radius; // 28 components
    mediump vec4 color; // 32 components
} ubuf;

layout(location = 0) in highp vec2 uv;
layout(location = 0) out mediump vec4 out_color;

const highp float minimum_radius = 0.05;

void main()
{
    highp vec4 clamped_radius = clamp(ubuf.radius * 2.0, 0.0, 1.0);

    mediump vec4 col = vec4(0.0);

    // Correction factor to round the corners of a larger shadow.
    // We want to account for size in regards to shadow radius, so that a larger shadow is
    // more rounded, but only if we are not already rounding the corners due to corner radius.
    highp vec4 size_factor = 0.5 * (minimum_radius / max(clamped_radius, minimum_radius));
    highp vec4 shadow_radius = clamped_radius + (ubuf.spread + ubuf.blur) * size_factor;

    // Calculate the shadow's distance field.
    highp float shadow = sdf_rounded_rectangle(uv, ubuf.aspect - ubuf.blur * 2.0, shadow_radius);

    col = mix(col, ubuf.color, 1.0 - smoothstep(-ubuf.blur * 2.0, ubuf.blur * 2.0, shadow));

    out_color = col * ubuf.opacity;
}

