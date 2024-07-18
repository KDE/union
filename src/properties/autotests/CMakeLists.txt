
file(GLOB _tests Test*.cpp)
ecm_add_tests(${_tests} LINK_LIBRARIES Qt6::Test Union::Union)
