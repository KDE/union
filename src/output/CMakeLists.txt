# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

add_subdirectory(qtquick)
add_subdirectory(qtwidgets)

if (WITH_KIRIGAMI)
    add_subdirectory(kirigami)
endif()
