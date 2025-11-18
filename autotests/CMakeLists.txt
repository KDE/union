# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

add_subdirectory(properties)

ecm_add_tests(
    input/TestPlasmaSvg.cpp
    TestStyleRule.cpp
    TestSelector.cpp
    TestTheme.cpp
    TestThemeRegistry.cpp
    TestColor.cpp
    LINK_LIBRARIES Qt6::Test Union::Union
)

if (WITH_CSS_INPUT)
    ecm_add_test(input/TestCss.cpp LINK_LIBRARIES Qt6::Test Union::Union cxx-rust-cssparser)
endif()

# TestTheme requires properly working selectors for it to function
set_tests_properties(TestSelector PROPERTIES FIXTURES_SETUP Selector)
set_tests_properties(TestTheme PROPERTIES FIXTURES_REQUIRED Selector)

# TestThemeRegistry requires properly working themes
set_tests_properties(TestTheme PROPERTIES FIXTURES_SETUP Theme)
set_tests_properties(TestThemeRegistry PROPERTIES FIXTURES_REQUIRED Theme)
