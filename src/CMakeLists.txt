# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

add_library(Union)
add_library(Union::Union ALIAS Union)

target_sources(Union PRIVATE
    ThemeRegistry.cpp
    ThemeRegistry.h
    Theme.cpp
    Theme.h
    ThemeLoader.h
    StyleRule.cpp
    StyleRule.h
    Element.cpp
    Element.h
    ElementQuery.cpp
    ElementQuery.h
    InputPlugin.cpp
    InputPlugin.h
    Selector.cpp
    Selector.h
    PropertiesTypes.h
)

ecm_qt_declare_logging_category(Union
    HEADER union_logging.h
    IDENTIFIER UNION_GENERAL
    CATEGORY_NAME union.general
    DEFAULT_SEVERITY Debug
    DESCRIPTION "Union - General"
    EXPORT Union
)

ecm_qt_declare_logging_category(Union
    HEADER union_query_logging.h
    IDENTIFIER UNION_QUERY
    CATEGORY_NAME union.query
    DEFAULT_SEVERITY Warning
    DESCRIPTION "Union - Query Debugging"
    EXPORT Union
)

ecm_generate_export_header(Union
    BASE_NAME Union
    GROUP_BASE_NAME KF
    VERSION ${PROJECT_VERSION}
    DEPRECATED_BASE_VERSION 0
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

set_target_properties(Union PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_link_libraries(Union PUBLIC
    Qt6::Core
    Qt6::Gui
)

target_include_directories(Union PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/Union>"
)

add_subdirectory(properties)

add_subdirectory(input)
add_subdirectory(output)

ecm_qt_install_logging_categories(EXPORT Union DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

ecm_generate_qdoc(Union ../doc/union.qdocconf)

install(TARGETS Union DESTINATION ${KDE_INSTALL_LIBDIR})
