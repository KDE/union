
find_package(Qt6Test REQUIRED)
find_package(Qt6QuickTest REQUIRED)

ecm_add_tests(
    input/TestPlasmaSvg.cpp
    TestStyle.cpp
    TestSelector.cpp
    LINK_LIBRARIES Qt6::Test Union::Union
)
