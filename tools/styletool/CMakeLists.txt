# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

add_executable(union-styletool)
target_sources(union-styletool PRIVATE styletool.cpp)

target_link_libraries(union-styletool PRIVATE Union::Union Qt::Core)

install(TARGETS union-styletool ${KDE_INSTALL_DEFAULT_ARGS})
