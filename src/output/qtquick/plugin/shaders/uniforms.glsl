// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

// Note: Due to alignment requirements of components, the order here is important.
// float are aligned on 1 component boundaries.
// vec2 on 2 component.
// everything else on 4 component.
layout(std140, binding = 0) uniform buf {
    highp mat4 matrix; // 16 components
    mediump float opacity; // 17 components

    mediump float size; // 18 components
    mediump float borderWidth; // 19 components
    mediump float outlineWidth; // 20 components

    mediump vec2 aspect; // 22 components
    mediump vec2 offset; // 24 components

    mediump vec4 radius; // 28 components

    mediump vec4 color; // 32 components
    mediump vec4 shadowColor; // 36 components
    mediump vec4 borderColor; // 40 components
    mediump vec4 outlineColor; // 44 components
} ubuf;

