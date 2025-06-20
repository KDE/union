# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

# Qt insists on some paths in qt_add_shaders to be target-relative while others
# are not, making it impossible to put this in the shaders/ subdirectory. To
# keep things a bit organised and easier to follow, this is still a separate
# file that gets included.

macro(add_shaders ARG_NAME)
    cmake_parse_arguments(ARG "" "INPUT" "DEFINES" ${ARGV})
    qt6_add_shaders(UnionQuickImpl "${ARG_NAME}"
        BATCHABLE
        PREFIX "/qt/qml/org/kde/union/impl/shaders"
        FILES
            "shaders/${ARG_INPUT}.vert"
            "shaders/${ARG_INPUT}.frag"
        OUTPUTS
            "${ARG_NAME}.vert.qsb"
            "${ARG_NAME}.frag.qsb"
        OUTPUT_TARGETS _targets
        DEFINES ${ARG_DEFINES}
        ${_extra_options}
    )
    install(TARGETS ${_targets} EXPORT KirigamiTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})
endmacro()

add_shaders("shadowed_rectangle"
    INPUT shadowedrectangle
)

add_shaders("shadowed_border_rectangle"
    INPUT shadowedrectangle
    DEFINES ENABLE_BORDER=1
)

add_shaders("shadowed_texture"
    INPUT shadowedrectangle
    DEFINES ENABLE_TEXTURE=1
)

add_shaders("shadowed_border_texture"
    INPUT shadowedrectangle
    DEFINES
        ENABLE_BORDER=1
        ENABLE_TEXTURE=1
)
