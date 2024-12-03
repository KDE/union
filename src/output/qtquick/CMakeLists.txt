
add_subdirectory(plugin)

add_library(UnionQuickStyle)
ecm_add_qml_module(UnionQuickStyle
    URI "org.kde.union"
    IMPORTS "org.kde.union.impl"
    GENERATE_PLUGIN_SOURCE
)

ecm_target_qml_sources(UnionQuickStyle SOURCES
    style/ApplicationWindow.qml
    style/Button.qml
    style/CheckBox.qml
    style/CheckDelegate.qml
    style/ItemDelegate.qml
    style/Menu.qml
    style/MenuItem.qml
    style/MenuSeparator.qml
    style/Page.qml
    style/Pane.qml
    style/ProgressBar.qml
    style/ScrollBar.qml
    style/ScrollIndicator.qml
    style/ScrollView.qml
    style/SpinBox.qml
    style/TabButton.qml
    style/TabBar.qml
    style/TextArea.qml
    style/TextField.qml
    style/ToolBar.qml
    style/ToolButton.qml
    style/ToolTip.qml
    style/ToolSeparator.qml
)

target_link_libraries(UnionQuickStyle PRIVATE UnionQuickImpl)

set_target_properties(UnionQuickStyle PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS UnionQuickStyle ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
ecm_finalize_qml_module(UnionQuickStyle)
