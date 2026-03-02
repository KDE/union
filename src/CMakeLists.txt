# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

add_library(Union)
add_library(Union::Union ALIAS Union)

target_sources(Union PRIVATE
    StyleRegistry.cpp
    Style.cpp
    StyleRule.cpp
    Element.cpp
    ElementQuery.cpp
    InputPlugin.cpp
    Selector.cpp
    Color.cpp
    PlatformPlugin.cpp
)

target_sources(Union PUBLIC
    FILE_SET HEADERS
    FILES
    StyleRegistry.h
    Style.h
    StyleLoader.h
    StyleRule.h
    Element.h
    ElementQuery.h
    InputPlugin.h
    Selector.h
    PropertiesTypes.h
    Color.h
    PluginRegistry.h
    PlatformPlugin.h
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
    "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/union>"
)

add_subdirectory(properties)

add_subdirectory(input)
add_subdirectory(output)
add_subdirectory(platform)

ecm_qt_install_logging_categories(EXPORT Union DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

ecm_generate_qdoc(Union ../doc/union.qdocconf)

install(TARGETS Union
    EXPORT UnionTargets
    FILE_SET HEADERS DESTINATION ${KDE_INSTALL_INCLUDEDIR}/union/Union
    ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)
