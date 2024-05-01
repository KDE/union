
add_library(UnionQuickImpl)
ecm_add_qml_module(UnionQuickImpl URI "org.kde.union.impl" NO_PLUGIN_OPTIONAL)

target_sources(UnionQuickImpl PRIVATE
    QuickElement.cpp
    QuickElement.h
    QuickStyle.cpp
    QuickStyle.h
    Background.cpp
    Background.h
    UnionNode.cpp
    UnionNode.h
    AreaNode.cpp
    AreaNode.h
    LineNode.cpp
    LineNode.h
    CornerNode.cpp
    CornerNode.h
    ShadowNode.cpp
    ShadowNode.h
    Types.h
    Types.cpp
)

target_sources(UnionQuickImplplugin PRIVATE
    UnionPlugin.cpp
)

target_link_libraries(UnionQuickImpl PUBLIC Qt6::Core Qt6::Qml Qt6::Quick Qt6::QuickControls2 Union::Union)

set_target_properties(UnionQuickImpl PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS UnionQuickImpl ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
ecm_finalize_qml_module(UnionQuickImpl)
