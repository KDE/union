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
include(CMakeDependentOption)

# General build options
option(BUILD_TOOLS "Build tools that are useful for development" ON)
option(BUILD_EXAMPLES "Build example applications" OFF)

# Options to control building platform integrations
option(BUILD_PLATFORM_FRAMEWORKS "Build KDE frameworks platform integration" ON)

# Options to control building inputs
option(BUILD_INPUT_CSS "Build the CSS input plugin" ON)
option(BUILD_INPUT_PLASMASVG "Build the PlasmaSVG input plugin" OFF)

# Options to control building outputs
option(BUILD_OUTPUT_QTQUICK "Build the QtQuick output" ON)
cmake_dependent_option(BUILD_OUTPUT_KIRIGAMI "Build the Kirigami output. Requires QtQuick output and Frameworks" ON "BUILD_QTQUICK;BUILD_PLATFORM_FRAMEWORKS" OFF)
# Temporarily disabled by default until we have more complete coverage
option(BUILD_OUTPUT_QTWIDGETS "Build the QtWidgets output" OFF)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

set(CMAKE_CXX_STANDARD 20)

# Find required packages

set(REQUIRED_QT_VERSION 6.9.0)
find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Gui)

if (BUILD_TESTING)
    find_package(Qt6Test REQUIRED)
    find_package(Qt6QuickTest REQUIRED)
endif()

if (BUILD_EXAMPLES)
    find_package(KF6Kirigami REQUIRED)
    set_package_properties(KF6Kirigami PROPERTIES TYPE REQUIRED PURPOSE "The gallery example uses Kirigami for its UI.")
endif()

if (BUILD_PLATFORM_FRAMEWORKS)
    find_package(KF6 ${KF_DEP_VERSION} REQUIRED COMPONENTS IconThemes ColorScheme GuiAddons)
    set_package_properties(KF6IconThemes PROPERTIES TYPE REQUIRED PURPOSE "KIconThemes is used for icon recoloring")
    set_package_properties(KF6ColorScheme PROPERTIES TYPE REQUIRED PURPOSE "KColorScheme is used to read system colors")
endif()

if (BUILD_OUTPUT_QTQUICK)
    find_package(Qt6Quick ${REQUIRED_QT_VERSION} CONFIG REQUIRED)
    find_package(Qt6QuickControls2 ${REQUIRED_QT_VERSION} CONFIG REQUIRED)
    find_package(Qt6ShaderTools ${REQUIRED_QT_VERSION} CONFIG REQUIRED)
endif()

if (BUILD_OUTPUT_QTWIDGETS)
    find_package(Qt6Widgets ${REQUIRED_QT_VERSION} CONFIG REQUIRED)
endif()

if (BUILD_OUTPUT_KIRIGAMI)
    find_package(KF6KirigamiPlatform ${KF_DEP_VERSION} CONFIG REQUIRED)
    set_package_properties(KF6KirigamiPlatform PROPERTIES TYPE REQUIRED PURPOSE "KirigamiPlatform provides the integration interface required by the Kirigami output")
endif()

if (BUILD_INPUT_CSS)
    find_package(cxx-rust-cssparser REQUIRED)
endif()

if (BUILD_INPUT_PLASMASVG)
    find_package(Qt6Svg ${REQUIRED_QT_VERSION} REQUIRED CONFIG)
    find_package(KF6 COMPONENTS Archive ColorScheme Config REQUIRED)
    find_package(Plasma REQUIRED)
    find_package(ryml REQUIRED)
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
