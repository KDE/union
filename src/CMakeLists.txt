
add_library(Union)

target_sources(Union PRIVATE
    Style.cpp
    StyleElement.cpp
    StyleLoader.cpp
    ElementIdentifier.cpp
)

ecm_qt_declare_logging_category(Union
    HEADER union_logging.h
    IDENTIFIER UNION_GENERAL
    CATEGORY_NAME union.general
    DEFAULT_SEVERITY Warning
    DESCRIPTION "Union - General"
    EXPORT Union
)

ecm_generate_export_header(Union
    BASE_NAME Union
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

set_target_properties(Union PROPERTIES
    VERSION ${KF_VERSION}
    SOVERSION 0
)

target_link_libraries(Union PUBLIC
    Qt6::Core
    Qt6::Gui

    Qt6::Svg
    KF6::Plasma
)

target_include_directories(Union PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/Union>"
)

add_subdirectory(input)
add_subdirectory(output)

ecm_qt_install_logging_categories(EXPORT Union DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

install(TARGETS Union DESTINATION ${KDE_INSTALL_LIBDIR})
