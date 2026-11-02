# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

ecm_add_test(TestControls.cpp
    ENVIRONMENT "UNION_DISABLE_CACHE=1" "QML_IMPORT_PATH=${CMAKE_BINARY_DIR}/bin"
    LINK_LIBRARIES Qt6::Test Qt6::Quick UnionQuickImpl
)
target_include_directories(TestControls PRIVATE ${CMAKE_SOURCE_DIR}/src/output/qtquick/plugin)
add_dependencies(TestControls UnionQuickImplplugin UnionQuickStyleplugin)

ecm_add_test(TestPositionerLayout.cpp LINK_LIBRARIES Qt6::Test Qt6::Quick Union::Union)

ecm_add_test(TEST_NAME TestMnemonics
    TestMnemonics.cpp
    ${CMAKE_SOURCE_DIR}/src/output/qtquick/plugin/Mnemonics.cpp
    LINK_LIBRARIES Qt6::Test Qt6::Quick Union::Union
)
target_include_directories(TestMnemonics PRIVATE ${CMAKE_SOURCE_DIR}/src/output/qtquick/plugin)
