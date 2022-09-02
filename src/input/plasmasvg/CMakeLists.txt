
add_library(union-plasmasvg)

target_sources(union-plasmasvg PRIVATE
    PlasmaSvgLoader.cpp
)

target_link_libraries(union-plasmasvg PRIVATE Union)
