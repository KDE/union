
file(GLOB _sources *.cpp)
file(GLOB _headers *.h)

target_sources(Union PRIVATE ${_sources} PUBLIC ${_headers})

if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()
