// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#version 440

layout(std140, binding = 0) uniform buf {
    highp mat4 matrix; // 16 components
    mediump float opacity; // 17 components + 1 padding

    mediump vec2 aspect; // 20 components

    mediump float spread; // 21 components
    mediump float blur; // 22 components + 2 padding
    mediump vec4 radius; // 28 components
    mediump vec4 color; // 32 components
} ubuf;

layout(location = 0) in highp vec4 in_vertex;
layout(location = 1) in highp vec2 in_uv;

layout(location = 0) out highp vec2 uv;

out gl_PerVertex { vec4 gl_Position; };

void main() {
    uv = (-1.0 + 2.0 * in_uv) * ubuf.aspect;
    gl_Position = ubuf.matrix * in_vertex;
}
