
add_library(UnionQuickImpl)
ecm_add_qml_module(UnionQuickImpl URI "org.kde.union.impl" GENERATE_PLUGIN_SOURCE)

target_sources(UnionQuickImpl PRIVATE
    QuickElement.cpp
    Background.cpp
    UnionNode.cpp
    AreaNode.cpp
    LineNode.cpp
    CornerNode.cpp
)

target_link_libraries(UnionQuickImpl PUBLIC Qt6::Core Qt6::Qml Qt6::Quick Qt6::QuickControls2 Union::Union)

set_target_properties(UnionQuickImpl PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS UnionQuickImpl ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
ecm_finalize_qml_module(UnionQuickImpl)
