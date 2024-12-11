# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

add_executable(union-widgets-gallery)
target_sources(union-widgets-gallery PRIVATE main.cpp)
target_link_libraries(union-widgets-gallery PRIVATE Qt::Widgets)

install(TARGETS union-widgets-gallery DESTINATION ${KDE_INSTALL_BINDIR})
