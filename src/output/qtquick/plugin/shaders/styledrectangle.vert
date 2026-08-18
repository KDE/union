// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#version 440

layout(std140, binding = 0) uniform buf {
    highp mat4 matrix; // 16 components
    mediump float opacity; // 17 components + 1 padding

    mediump vec2 aspect; // 20 components

    mediump vec4 borderWidth; // 24 components
    mediump vec4 outlineWidth; // 28 components
    mediump vec4 radius; // 32 components
} ubuf;

layout(location = 0) in highp vec4 in_vertex;
layout(location = 1) in highp vec2 in_uv0;
layout(location = 2) in mediump vec4 in_border_color;
layout(location = 3) in mediump vec4 in_outline_color;

layout(location = 0) out highp vec2 uv0;
layout(location = 1) out mediump vec4 border_color;
layout(location = 2) out mediump vec4 outline_color;

out gl_PerVertex { vec4 gl_Position; };

void main() {
    uv0 = (-1.0 + 2.0 * in_uv0) * ubuf.aspect;
    border_color = in_border_color;
    outline_color = in_outline_color;
    gl_Position = ubuf.matrix * in_vertex;
}
