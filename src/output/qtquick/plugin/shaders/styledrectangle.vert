// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#version 440

#extension GL_GOOGLE_include_directive: enable
#include "uniforms.glsl"

layout(location = 0) in highp vec4 in_vertex;
layout(location = 1) in highp vec2 in_uv0;
layout(location = 2) in mediump vec2 in_uv1;
layout(location = 3) in mediump vec4 in_border_color;
layout(location = 4) in mediump vec4 in_outline_color;

layout(location = 0) out highp vec2 uv0;
layout(location = 1) out mediump vec2 uv1;
layout(location = 2) out mediump vec4 border_color;
layout(location = 3) out mediump vec4 outline_color;

out gl_PerVertex { vec4 gl_Position; };

void main() {
    uv0 = (-1.0 + 2.0 * in_uv0) * ubuf.aspect;
    uv1 = in_uv1;
    border_color = in_border_color;
    outline_color = in_outline_color;
    gl_Position = ubuf.matrix * in_vertex;
}
