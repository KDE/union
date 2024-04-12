
add_library(union-plasmasvg MODULE)

target_sources(union-plasmasvg PRIVATE
    PlasmaSvgLoader.cpp
    PlasmaSvgPlugin.cpp
    PlasmaSvgPlugin.qrc
)

target_link_libraries(union-plasmasvg PRIVATE Union Qt6::Svg Plasma::Plasma ryml::ryml)

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
