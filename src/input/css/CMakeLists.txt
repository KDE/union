# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

add_library(union-input-css MODULE)

target_sources(union-input-css PRIVATE
    CssPlugin.cpp
    CssPlugin.h
    CssLoader.cpp
    CssLoader.h
)

target_link_libraries(union-input-css PRIVATE
    Union
    cxx-rust-cssparser
)

ecm_qt_declare_logging_category(union-input-css
    HEADER css_logging.h
    IDENTIFIER UNION_CSS
    CATEGORY_NAME union.css
    DESCRIPTION "Union - CSS Input Plugin"
    EXPORT Union
)

kde_target_enable_exceptions(union-input-css PRIVATE)

set_target_properties(union-input-css PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/union
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/union
)

install(TARGETS union-input-css DESTINATION ${KDE_INSTALL_PLUGINDIR}/union)

set(_union_data_dir "${KDE_INSTALL_DATADIR}/union")
install(DIRECTORY defaults DESTINATION ${_union_data_dir}/css)
install(DIRECTORY styles DESTINATION ${_union_data_dir})
