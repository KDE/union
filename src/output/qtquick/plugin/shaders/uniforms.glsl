// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

// Note: Due to alignment requirements of components, the order here is important.
// float are aligned on 1 component boundaries.
// vec2 on 2 component.
// everything else on 4 component.
layout(std140, binding = 0) uniform buf {
    highp mat4 matrix; // 16 components
    mediump float opacity; // 17 components + 1 padding

    highp float inverseScale; // 20 components
    mediump vec4 borderWidth; // 24 components
    mediump vec4 outlineWidth; // 28 components
} ubuf;
