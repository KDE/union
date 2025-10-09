# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

add_subdirectory(plugin)

add_library(UnionQuickStyle)
ecm_add_qml_module(UnionQuickStyle
    URI "org.kde.union"
    IMPORTS "org.kde.union.impl"
    GENERATE_PLUGIN_SOURCE
)


# Workaround for QTBUG-139626
if (${Qt6Core_VERSION} VERSION_GREATER_EQUAL "6.9.2")
    set_target_properties(UnionQuickStyle PROPERTIES
        QT_QMLCACHEGEN_ARGUMENTS "--only-bytecode"
    )
endif()

ecm_target_qml_sources(UnionQuickStyle SOURCES
    style/AbstractButton.qml
    style/Action.qml
    style/ActionGroup.qml
    style/ApplicationWindow.qml
    style/BusyIndicator.qml
    style/Button.qml
    style/CheckBox.qml
    style/CheckDelegate.qml
    style/ComboBox.qml
    style/Container.qml
    style/Control.qml
    style/Dialog.qml
    style/DialogButtonBox.qml
    style/Drawer.qml
    style/Frame.qml
    style/GroupBox.qml
    style/HorizontalHeaderView.qml
    style/ItemDelegate.qml
    style/Label.qml
    style/Menu.qml
    style/MenuBar.qml
    style/MenuBarItem.qml
    style/MenuItem.qml
    style/MenuSeparator.qml
    style/Page.qml
    style/Pane.qml
    style/Popup.qml
    style/ProgressBar.qml
    style/RadioButton.qml
    style/RadioDelegate.qml
    style/RangeSlider.qml
    style/ScrollBar.qml
    style/ScrollIndicator.qml
    style/ScrollView.qml
    style/SelectionRectangle.qml
    style/Slider.qml
    style/SpinBox.qml
    style/SplitView.qml
    style/StackView.qml
    style/SwipeDelegate.qml
    style/SwipeView.qml
    style/Switch.qml
    style/SwitchDelegate.qml
    style/TabBar.qml
    style/TabButton.qml
    style/TableViewDelegate.qml
    style/TreeViewDelegate.qml
    style/TextArea.qml
    style/TextField.qml
    style/ToolBar.qml
    style/ToolButton.qml
    style/ToolSeparator.qml
    style/ToolTip.qml
    style/VerticalHeaderView.qml
)

target_link_libraries(UnionQuickStyle PRIVATE UnionQuickImpl)

set_target_properties(UnionQuickStyle PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS UnionQuickStyle ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
ecm_finalize_qml_module(UnionQuickStyle)
