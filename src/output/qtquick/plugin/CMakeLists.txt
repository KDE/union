# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

add_library(UnionQuickImpl)
ecm_add_qml_module(UnionQuickImpl URI "org.kde.union.impl"
    NO_PLUGIN_OPTIONAL
    DEPENDENCIES QtQuick
)

target_sources(UnionQuickImpl PRIVATE
    QuickElement.cpp
    QuickElement.h
    QuickStyle.cpp
    QuickStyle.h
    Types.h
    Types.cpp
    StyledRectangle.cpp
    StyledRectangle.h
    Positioner.h
    Positioner.cpp
    PositionerLayout.h
    PositionerLayout.cpp

    scenegraph/UnionNode.cpp
    scenegraph/UnionNode.h
    scenegraph/AreaNode.cpp
    scenegraph/AreaNode.h
    scenegraph/LineNode.cpp
    scenegraph/LineNode.h
    scenegraph/CornerNode.cpp
    scenegraph/CornerNode.h
    scenegraph/ShadowNode.cpp
    scenegraph/ShadowNode.h
    scenegraph/RectangleNode.cpp
    scenegraph/RectangleNode.h

    scenegraph/ShaderNode.cpp
    scenegraph/ShaderNode.h
    scenegraph/ShaderMaterial.cpp
    scenegraph/ShaderMaterial.h
    scenegraph/TextureCache.cpp
    scenegraph/TextureCache.h
    scenegraph/UniformDataStream.h
)

target_sources(UnionQuickImplplugin PRIVATE
    UnionPlugin.cpp
)

add_subdirectory(properties)

include(shaders.cmake)

ecm_qt_declare_logging_category(UnionQuickImpl
    HEADER qtquick_logging.h
    IDENTIFIER UNION_QTQUICK
    CATEGORY_NAME union.qtquick
    DEFAULT_SEVERITY Debug
    DESCRIPTION "Union - QtQuick Output Plugin"
    EXPORT Union
)

target_include_directories(UnionQuickImpl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/properties)
target_link_libraries(UnionQuickImpl PUBLIC Qt6::Core Qt6::Qml Qt6::Quick Qt6::QuickControls2 Union::Union)

set_target_properties(UnionQuickImpl PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS UnionQuickImpl ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
ecm_finalize_qml_module(UnionQuickImpl)
