# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "6.0.0") # handled by release scripts
set(KF_DEP_VERSION "6.0.0") # handled by release scripts

project(Union VERSION ${KF_VERSION})

include(FeatureSummary)
find_package(ECM ${KF_DEP_VERSION} NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(KDEGitCommitHooks)
include(ECMGenerateExportHeader)
include(ECMAddTests)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)
include(ECMQmlModule)
include(ECMGenerateQDoc)

option(BUILD_TOOLS "Build tools that are useful for development" ON)
option(BUILD_EXAMPLES "Build example applications" OFF)
option(WITH_FRAMEWORKS "Build features dependant on KDE frameworks" ON)
option(WITH_KIRIGAMI "Build with Kirigami integration" ON)

if (WITH_KIRIGAMI AND NOT WITH_KICONTHEMES)
    message(STATUS "KIconThemes was disabled but Kirigami enabled, forcing KIconThemes as it is required for Kirigami output")
    set(WITH_KICONTHEMES ON)
endif()

set(CMAKE_CXX_STANDARD 20)

set(REQUIRED_QT_VERSION 6.8.0)
find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Gui Quick QuickControls2 Widgets ShaderTools)

if (WITH_FRAMEWORKS)
    find_package(KF6 ${KF_DEP_VERSION} REQUIRED COMPONENTS IconThemes ColorScheme)
    set_package_properties(KF6IconThemes PROPERTIES TYPE REQUIRED PURPOSE "KIconThemes is used for icon recoloring")
    set_package_properties(KF6ColorScheme PROPERTIES TYPE REQUIRED PURPOSE "KColorScheme is used to read system colors")
endif()

if (WITH_KIRIGAMI)
    find_package(KF6KirigamiPlatform ${KF_DEP_VERSION} CONFIG REQUIRED)
endif()

if (BUILD_EXAMPLES)
    find_package(KF6Kirigami REQUIRED)
    set_package_properties(KF6Kirigami PROPERTIES TYPE REQUIRED PURPOSE "The gallery example uses Kirigami for its UI.")
endif()

if (BUILD_TESTING)
    find_package(Qt6Test REQUIRED)
    find_package(Qt6QuickTest REQUIRED)
endif()

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

option(WITH_PLASMASVG_INPUT "Enable Plasma SVG input plugin" OFF)
if (WITH_PLASMASVG_INPUT)
    find_package(Qt6Svg ${REQUIRED_QT_VERSION} REQUIRED CONFIG)
    find_package(KF6 COMPONENTS Archive ColorScheme Config REQUIRED)
    find_package(Plasma REQUIRED)
    find_package(ryml REQUIRED)
endif()

option(WITH_CSS_INPUT "Enable CSS input plugin" ON)
if (WITH_CSS_INPUT)
    find_package(cxx-rust-cssparser REQUIRED)
endif()

add_subdirectory(src)

if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()

ecm_setup_version(
    PROJECT
    VARIABLE_PREFIX Union
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/union_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/UnionConfigVersion.cmake"
    SOVERSION 1
)

configure_package_config_file(
    "UnionConfig.cmake.in"
    "UnionConfig.cmake"
    INSTALL_DESTINATION ${KDE_INSTALL_CMAKEPACKAGEDIR}/Union
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/UnionConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/UnionConfigVersion.cmake"
    DESTINATION ${KDE_INSTALL_CMAKEPACKAGEDIR}/Union
    COMPONENT Devel
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
