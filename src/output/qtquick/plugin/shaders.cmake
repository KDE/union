# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

# Qt insists on some paths in qt_add_shaders to be target-relative while others
# are not, making it impossible to put this in the shaders/ subdirectory. To
# keep things a bit organised and easier to follow, this is still a separate
# file that gets included.

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(_extra_options DEBUGINFO)
else()
    set(_extra_options PRECOMPILE OPTIMIZED)
endif()

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

macro(add_variants ARG_NAME)
    cmake_parse_arguments(ARG "" "INPUT" "VARIANTS" ${ARGV})

    add_shaders("${ARG_NAME}" INPUT "${ARG_NAME}")

    message(STATUS ${ARG_VARIANTS})
    foreach(_variant ${ARG_VARIANTS})
        string(REPLACE "-" ";" _parts "${_variant}")

        set(_defines "")
        foreach(_part ${_parts})
            string(TOUPPER "${_part}" _part_uc)
            list(APPEND _defines "ENABLE_${_part_uc}=1")
        endforeach()

        add_shaders("${ARG_NAME}-${_variant}"
            INPUT "${ARG_NAME}"
            DEFINES ${_defines}
        )
    endforeach()
endmacro()

add_shaders("rectangleshadow" INPUT rectangleshadow)

add_variants("styledrectangle" INPUT styledrectangle VARIANTS
    "border"
    "border-outline"
    "border-texture"
    "border-outline-texture"
    "outline"
    "outline-texture"
    "texture"
    "mask"
    "texture-mask"
    "border-texture-mask"
    "border-outline-texture-mask"
    "invertedmask"
    "texture-invertedmask"
    "border-texture-invertedmask"
    "border-outline-texture-invertedmask"
)

