
if(ENABLE_TESTS)
    include(CTest)

    list(APPEND CMAKE_MODULE_PATH ${CONAN_LIB_DIRS_GTEST}/cmake/gtest)
    include(GoogleTest)
endif()

macro(discover_tests target)
    string(REGEX MATCH "(.*)-test" _match ${target})
    gtest_discover_tests(${target} TEST_PREFIX ${CMAKE_MATCH_1}:)
    # This will create a preprocessor macro named `TEST_DATA_DIR` that can be used within tests.
    # Useful for finding the absolute path of a file.
    target_compile_definitions(${target} PUBLIC TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/data/")
endmacro()

# macro(add_project_test target)
#     gtest_add_tests(${target})
#     target_compile_definitions(${target} PUBLIC TEST_RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR/data")
# endmacro()