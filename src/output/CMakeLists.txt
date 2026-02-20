# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

if (BUILD_OUTPUT_QTQUICK)
    add_subdirectory(qtquick)
endif()

if (BUILD_OUTPUT_QTWIDGETS)
    add_subdirectory(qtwidgets)
endif()

if (BUILD_OUTPUT_KIRIGAMI)
    add_subdirectory(kirigami)
endif()
