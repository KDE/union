// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#version 440

#extension GL_GOOGLE_include_directive: enable
#include "sdf.glsl"
// See sdf.glsl for the SDF related functions.

// This shader renders a rectangle with rounded corners and a shadow below it.

#include "uniforms.glsl"

#ifdef ENABLE_TEXTURE
layout(binding = 1) uniform sampler2D textureSource;
#endif

layout(location = 0) in lowp vec2 uv;
layout(location = 0) out lowp vec4 out_color;

const lowp float minimum_shadow_radius = 0.05;

void main()
{
    lowp vec4 clamped_radius = clamp(ubuf.radius * 2.0, 0.0, 1.0);

    lowp vec4 col = vec4(0.0);

#ifndef ENABLE_LOWPOWER
    // Scaling factor that is the inverse of the amount of scaling applied to the geometry.
    lowp float inverse_scale = 1.0 / (1.0 + max(ubuf.size + length(ubuf.offset) * 2.0,  ubuf.outlineWidth * 2.0));

    // Correction factor to round the corners of a larger shadow.
    // We want to account for size in regards to shadow radius, so that a larger shadow is
    // more rounded, but only if we are not already rounding the corners due to corner radius.
    lowp vec4 size_factor = 0.5 * (minimum_shadow_radius / max(clamped_radius, minimum_shadow_radius));
    lowp vec4 shadow_radius = clamped_radius + ubuf.size * size_factor;

    // Calculate the shadow's distance field.
    lowp float shadow = sdf_rounded_rectangle(uv - ubuf.offset * 2.0 * inverse_scale, ubuf.aspect * inverse_scale, shadow_radius * inverse_scale);
    // Render it, interpolating the color over the distance.
    col = mix(col, ubuf.shadowColor * sign(ubuf.size), 1.0 - smoothstep(-ubuf.size * 0.5, ubuf.size * 0.5, shadow));
#else
    lowp float inverse_scale = 1.0;
#endif

    // Scale corrected corner radius
    lowp vec4 corner_radius = clamped_radius * inverse_scale;

    lowp float rect = sdf_rounded_rectangle(uv, ubuf.aspect * inverse_scale, corner_radius);

#ifdef ENABLE_OUTLINE
    col = sdf_render(rect - (ubuf.outlineWidth * inverse_scale) * 2.0, col, ubuf.outlineColor);
#endif

#ifdef ENABLE_BORDER
    col = sdf_render(rect, col, ubuf.borderColor);

    // The inner rectangle distance field is the outer reduced by twice the border size.
    rect = rect + (ubuf.borderWidth * inverse_scale) * 2.0;
#endif
    // Finally, render the inner rectangle.
    col = sdf_render(rect, col, ubuf.color);

#ifdef ENABLE_TEXTURE
    // Sample the texture, then blend it on top of the background color.
    lowp vec2 texture_uv = ((uv / ubuf.aspect) + (1.0 * inverse_scale)) / (2.0 * inverse_scale);
    lowp vec4 texture_color = texture(textureSource, texture_uv);
    col = sdf_render(rect, col, texture_color, texture_color.a, sdf_default_smoothing);
#endif

    out_color = col * ubuf.opacity;
}
