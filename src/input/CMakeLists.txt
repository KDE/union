# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

if (WITH_PLASMASVG_INPUT)
    add_subdirectory(plasmasvg)
endif()

if (WITH_CSS_INPUT)
    add_subdirectory(css)
endif()
