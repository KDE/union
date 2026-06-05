# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

add_library(UnionWidgetsStyle MODULE)
target_sources(UnionWidgetsStyle PRIVATE
        StyleDrawing.cpp
        StyleDrawing.h
        StyleUtils.cpp
        StyleUtils.h
        UnionStyle.cpp
        UnionStyle.h
        UnionStylePlugin.cpp
        UnionStylePlugin.h
        Types.cpp
        Types.h
        Positioner.h
        Positioner.cpp
        positioner/PositionerLayout.h
        positioner/PositionerLayout.cpp
)
target_link_libraries(UnionWidgetsStyle
        PUBLIC
        Qt::Widgets
        Union::Union
)

set_target_properties(UnionWidgetsStyle PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS UnionWidgetsStyle DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/styles/)
install(FILES union.themerc DESTINATION ${KDE_INSTALL_DATADIR}/kstyle/themes)

