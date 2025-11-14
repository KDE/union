# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
# SPDX-FileCopyrightText: 2025 Martin Sh <hemisputnik@proton.me>

add_executable(union-widgets-gallery)

target_sources(union-widgets-gallery PRIVATE
    main.cpp
    mainwindow.cpp
    widgetpage.cpp
    buttonspage.cpp
    textpage.cpp
    numberpage.cpp
    checkablepage.cpp
    listpage.cpp
)

target_link_libraries(union-widgets-gallery PRIVATE Qt::Widgets)

install(TARGETS union-widgets-gallery DESTINATION ${KDE_INSTALL_BINDIR})
