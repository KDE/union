# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

add_library(UnionQuickPrivate)
ecm_add_qml_module(UnionQuickPrivate URI "org.kde.union.private"
    GENERATE_PLUGIN_SOURCE
    DEPENDENCIES QtQuick org.kde.union.impl
)

ecm_target_qml_sources(UnionQuickPrivate SOURCES
    DefaultContentItem.qml
    DefaultIndicator.qml
    DisplayAttribute.qml
    PlaceholderText.qml
    SpinBoxHelper.qml
    TextFieldContextMenu.qml
)

target_link_libraries(UnionQuickPrivate PRIVATE UnionQuickImpl)

set_target_properties(UnionQuickPrivate PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS UnionQuickPrivate ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
ecm_finalize_qml_module(UnionQuickPrivate)
