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
    lowp vec4 clamped_radius = clamp(ubuf.radius * 2.0, 0.0, 1.0);

    lowp vec4 col = vec4(0.0);

    // Scaling factor that is the inverse of the amount of scaling applied to the geometry.
    highp float inverse_scale = ubuf.inverseScale;

#ifdef ENABLE_SHADOW
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

    mediump vec4 rect = vec4(uv, ubuf.aspect * inverse_scale);

#ifdef ENABLE_OUTLINE
    rect = adjusted_rect(rect, -ubuf.outlineWidth * inverse_scale);
    corner_radius = adjusted_radius(corner_radius, -ubuf.outlineWidth * inverse_scale);
    col = sdf_render(sdf_rounded_rectangle(rect.xy, rect.zw, corner_radius), col, ubuf.outlineColor);

    rect = vec4(uv, ubuf.aspect * inverse_scale);
    corner_radius = clamped_radius * inverse_scale;
#endif

#ifdef ENABLE_BORDER
    col = sdf_render(sdf_rounded_rectangle(rect.xy, rect.zw, corner_radius), col, ubuf.borderColor);
    rect = adjusted_rect(rect, ubuf.borderWidth * inverse_scale);

    // Adjust corner radius for the amount the border makes the inner rectangle
    // smaller.
    corner_radius = adjusted_radius(corner_radius, ubuf.borderWidth * inverse_scale);
#endif
    // Finally, render the inner rectangle.
    mediump float sdf = sdf_rounded_rectangle(rect.xy, rect.zw, corner_radius);
    col = sdf_render(sdf, col, ubuf.color);

#ifdef ENABLE_TEXTURE
    // Sample the texture, then blend it on top of the background color.
    lowp vec2 texture_uv = ((uv / ubuf.aspect) + (1.0 * inverse_scale)) / (2.0 * inverse_scale);
    lowp vec4 texture_color = texture(textureSource, texture_uv);
    col = sdf_render(sdf, col, texture_color, texture_color.a, sdf_default_smoothing);
#endif

    out_color = col * ubuf.opacity;
}
