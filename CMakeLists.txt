cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "5.96.0") # handled by release scripts
set(KF_DEP_VERSION "5.95.0") # handled by release scripts

project(Union VERSION ${KF_VERSION})

include(FeatureSummary)
find_package(ECM ${KF_DEP_VERSION} NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(KDEGitCommitHooks)
include(ECMGenerateExportHeader)
include(ECMAddTests)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)
include(ECMQmlModule)

option(BUILD_EXAMPLES "Build example applications" OFF)

set(REQUIRED_QT_VERSION 5.15.2)
find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Gui)

find_package(Qt${QT_MAJOR_VERSION}Quick ${REQUIRED_QT_VERSION} CONFIG)
find_package(Qt${QT_MAJOR_VERSION}Widgets ${REQUIRED_QT_VERSION} CONFIG)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

add_subdirectory(src)

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
