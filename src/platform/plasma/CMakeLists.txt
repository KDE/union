# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

add_library(union-platform-plasma MODULE)
target_sources(union-platform-plasma PRIVATE
    PlasmaPlatformPlugin.cpp
    PlasmaPlatformPlugin.h
)

kconfig_target_kcfg_file(union-platform-plasma
    FILE unionsettings.kcfg
    CLASS_NAME UnionSettings
    GENERATE_MOC
    SINGLETON
)

target_link_libraries(union-platform-plasma PRIVATE Union KF6::CoreAddons KF6::ConfigGui KF6::IconThemes Qt6::DBus)

set_target_properties(union-platform-plasma PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/union
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/union
)

install(TARGETS union-platform-plasma DESTINATION ${KDE_INSTALL_PLUGINDIR}/union)
