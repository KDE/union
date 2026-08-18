// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#version 440

#extension GL_GOOGLE_include_directive: enable
#include "sdf.glsl"
// See sdf.glsl for the SDF related functions.

#if defined(ENABLE_TEXTURE) || defined(ENABLE_MASK) || defined(ENABLE_INVERTEDMASK)
    #define WITH_TEXTURE 1
#endif

layout(std140, binding = 0) uniform buf {
    highp mat4 matrix; // 16 components
    mediump float opacity; // 17 components + 1 padding
    mediump vec2 aspect; // 20 components
    mediump vec4 color; // 24 components
    mediump vec4 maskColor; // 28 components
    mediump vec4 radius; // 32 components
} ubuf;

#ifdef WITH_TEXTURE
layout(binding = 1) uniform sampler2D textureSource;
#endif

layout(location = 0) in highp vec2 uv0;
layout(location = 1) in mediump vec2 uv1;

layout(location = 0) out lowp vec4 out_color;

void main()
{
    mediump vec4 col = vec4(0.0);

    mediump vec4 rect = vec4(uv0, ubuf.aspect);

    highp vec4 corner_radius = clamp(ubuf.radius * 2.0, 0.0, 1.0);

    mediump float sdf = sdf_rounded_rectangle(rect.xy, rect.zw, corner_radius);
    col = sdf_render(sdf, col, ubuf.color);

#ifdef WITH_TEXTURE
    // Sample the texture, then blend it on top of the background color.
    mediump vec4 texture_color = texture(textureSource, uv1);
#ifdef ENABLE_MASK
    texture_color = vec4(ubuf.maskColor.xyz * texture_color.a, texture_color.a);
#endif
#ifdef ENABLE_INVERTEDMASK
    texture_color = vec4(ubuf.maskColor.xyz * (1 - texture_color.a), (1 - texture_color.a));
#endif
    col = sdf_render(sdf, col, texture_color, texture_color.a, sdf_default_smoothing);
#endif

    out_color = col * ubuf.opacity;
}
