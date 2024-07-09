
find_package(Qt6Test REQUIRED)
find_package(Qt6QuickTest REQUIRED)

ecm_add_tests(
    input/TestPlasmaSvg.cpp
    TestStyleRule.cpp
    TestSelector.cpp
    TestTheme.cpp
    LINK_LIBRARIES Qt6::Test Union::Union
)

# TestTheme requires properly working selectors for it to function
set_tests_properties(TestSelector PROPERTIES FIXTURES_SETUP Selector)
set_tests_properties(TestTheme PROPERTIES FIXTURES_REQUIRED Selector)
