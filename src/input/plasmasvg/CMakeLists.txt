
add_library(union-plasmasvg MODULE)

target_sources(union-plasmasvg PRIVATE
    PlasmaSvgLoader.cpp
    PlasmaSvgPlugin.cpp
)

target_link_libraries(union-plasmasvg PRIVATE Union Qt6::Svg KF6::Svg)

set_target_properties(union-plasmasvg PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/union
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/union
)

install(TARGETS union-plasmasvg DESTINATION ${KDE_INSTALL_PLUGINDIR}/union)
