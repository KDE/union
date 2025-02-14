# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

add_library(union-css MODULE)

target_sources(union-css PRIVATE
    CssPlugin.cpp
    CssPlugin.h
    CssLoader.cpp
    CssLoader.h
)

target_link_libraries(union-css PRIVATE
    Union
    cxx-rust-cssparser
)

ecm_qt_declare_logging_category(union-css
    HEADER css_logging.h
    IDENTIFIER UNION_CSS
    CATEGORY_NAME union.css
    DEFAULT_SEVERITY Debug
    DESCRIPTION "Union - CSS Input Plugin"
    EXPORT Union
)

kde_target_enable_exceptions(union-css PRIVATE)

set_target_properties(union-css PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/union
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/union
)

install(TARGETS union-css DESTINATION ${KDE_INSTALL_PLUGINDIR}/union)

