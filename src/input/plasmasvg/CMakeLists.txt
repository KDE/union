
add_library(union-plasmasvg MODULE)

add_custom_target(generate-definitions
    COMMAND "${CMAKE_COMMAND}" -D OUTPUT_DIR="${CMAKE_CURRENT_BINARY_DIR}" -P GenerateDefinitionsResource.cmake
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/PlasmaSvgDefinitions.qrc
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
add_dependencies(union-plasmasvg generate-definitions)

target_sources(union-plasmasvg PRIVATE
    PlasmaSvgLoader.cpp
    PlasmaSvgLoader.h
    PlasmaSvgPlugin.cpp
    PlasmaSvgPlugin.h
    PlasmaSvgRenderer.cpp
    PlasmaSvgRenderer.h
    LoadingContext.cpp
    LoadingContext.h
    PropertyFunctions.cpp
    PropertyFunctions.h
    ${CMAKE_CURRENT_BINARY_DIR}/PlasmaSvgDefinitions.qrc
)

target_link_libraries(union-plasmasvg PRIVATE
    Union
    Qt6::Svg
    KF6::Archive
    KF6::ColorScheme
    Plasma::Plasma
    ryml::ryml
)

ecm_qt_declare_logging_category(union-plasmasvg
    HEADER plasmasvg_logging.h
    IDENTIFIER UNION_PLASMASVG
    CATEGORY_NAME union.plasmasvg
    DEFAULT_SEVERITY Debug
    DESCRIPTION "Union - Plasma SVG Input Plugin"
    EXPORT Union
)

kde_target_enable_exceptions(union-plasmasvg PRIVATE)

set_target_properties(union-plasmasvg PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/union
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/union
)

install(TARGETS union-plasmasvg DESTINATION ${KDE_INSTALL_PLUGINDIR}/union)
