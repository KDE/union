# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

add_library(union-colorprovider-kcolorscheme MODULE)
target_sources(union-colorprovider-kcolorscheme PRIVATE
    KColorSchemeProvider.h
    KColorSchemeProvider.cpp
)

target_link_libraries(union-colorprovider-kcolorscheme PRIVATE Union KF6::ColorScheme KF6::GuiAddons)

ecm_qt_declare_logging_category(union-colorprovider-kcolorscheme
    HEADER kcolorscheme_logging.h
    IDENTIFIER UNION_KCOLORSCHEME
    CATEGORY_NAME union.kcolorscheme
    DEFAULT_SEVERITY Warning
    DESCRIPTION "Union - KColorScheme Color Provider Plugin"
    EXPORT Union
)


set_target_properties(union-colorprovider-kcolorscheme PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/union
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/union
)

install(TARGETS union-colorprovider-kcolorscheme DESTINATION ${KDE_INSTALL_PLUGINDIR}/union)
