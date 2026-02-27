# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

add_library(UnionPlatformShared)
target_sources(UnionPlatformShared PRIVATE
    KColorSchemeProvider.h
    KColorSchemeProvider.cpp
)

ecm_qt_declare_logging_category(UnionPlatformShared
    HEADER platform_logging.h
    IDENTIFIER UNION_PLATFORM
    CATEGORY_NAME union.platform
    DEFAULT_SEVERITY Warning
    DESCRIPTION "Union - Plafrom Integration"
    EXPORT Union
)

ecm_generate_export_header(UnionPlatformShared
    BASE_NAME UnionPlatform
    GROUP_BASE_NAME KF
    VERSION ${PROJECT_VERSION}
    DEPRECATED_BASE_VERSION 0
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

target_link_libraries(UnionPlatformShared PUBLIC Union PRIVATE KF6::ColorScheme KF6::GuiAddons)

install(TARGETS UnionPlatformShared ${KDE_INSTALL_DEFAULT_ARGS})
