
if(ENABLE_TESTS)
    include(CTest)

    list(APPEND CMAKE_MODULE_PATH ${CONAN_LIB_DIRS_GTEST}/cmake/gtest)
    include(GoogleTest)
endif()

macro(discover_tests target)
    string(REGEX MATCH "(.*)-test" _match ${target})
    gtest_discover_tests(${target}
        TEST_PREFIX ${CMAKE_MATCH_1}:)
endmacro()