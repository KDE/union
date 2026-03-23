# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

if (BUILD_WITH_KDEFRAMEWORKS)
    add_subdirectory(kcolorschemeprovider)
endif()

if (BUILD_PLATFORM_PLASMA)
    add_subdirectory(plasma)
endif()
