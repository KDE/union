# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

if (BUILD_INPUT_CSS)
    add_subdirectory(css)
endif()

if (BUILD_INPUT_PLASMASVG)
    add_subdirectory(plasmasvg)
endif()
