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

add_shaders("rectangleshadow" INPUT rectangleshadow)

macro(name_to_define ARG_NAME ARG_OUTPUT)
    if ("${ARG_NAME}" STREQUAL "border")
        set(${ARG_OUTPUT} ENABLE_BORDER=1)
    endif()
    if ("${ARG_NAME}" STREQUAL "outline")
        set(${ARG_OUTPUT} ENABLE_OUTLINE=1)
    endif()
    if ("${ARG_NAME}" STREQUAL "texture")
        set(${ARG_OUTPUT} ENABLE_TEXTURE=1)
    endif()
endmacro()

set(_variants
    "border"
    "border-outline"
    "border-texture"
    "border-outline-texture"
    "outline"
    "outline-texture"
    "texture"
)

add_shaders("styledrectangle" INPUT styledrectangle)

foreach(_variant ${_variants})
    string(REPLACE "-" ";" _parts "${_variant}")

    set(_defines "")
    foreach(_part ${_parts})
        name_to_define("${_part}" _define)
        list(APPEND _defines "${_define}")
    endforeach()

    add_shaders("styledrectangle-${_variant}"
        INPUT styledrectangle
        DEFINES ${_defines}
    )
endforeach()
