
ecm_add_qml_module(UnionQmlPlugin URI "org.kde.union")

target_sources(UnionQmlPlugin PRIVATE
    Element.cpp
    Background.cpp
    UnionPlugin.cpp
)
target_link_libraries(UnionQmlPlugin PRIVATE Qt6::Core Qt6::Quick Qt6::Qml Union::Union)

ecm_target_qml_sources(UnionQmlPlugin SOURCES
    qml/Button.qml
)

ecm_finalize_qml_module(UnionQmlPlugin DESTINATION ${KDE_INSTALL_QMLDIR})
