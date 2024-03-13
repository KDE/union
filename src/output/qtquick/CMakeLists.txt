
ecm_add_qml_module(UnionQmlPlugin URI "org.kde.union" GENERATE_PLUGIN_SOURCE)

target_sources(UnionQmlPlugin PRIVATE
    # UnionPlugin.cpp
    Element.cpp
    Background.cpp
)
target_link_libraries(UnionQmlPlugin PRIVATE Qt6::Core Qt6::Quick Qt6::Qml Union::Union)

ecm_target_qml_sources(UnionQmlPlugin SOURCES
    qml/Button.qml
)

ecm_finalize_qml_module(UnionQmlPlugin DESTINATION ${KDE_INSTALL_QMLDIR})
