cmake_minimum_required (VERSION 2.8.12)

project(musher LANGUAGES CXX)

# Options. Turn on with 'cmake -Dmyvarname=ON'.
option(PACKAGE_TESTS "Build the tests" ON) # Makes boolean 'test' available.

# Add /tools to cmake path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/tools")

# Include buildTools cmake
include(buildTools)

# Include useful testing variables, only when this is the main CMakeList
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

# Set variable for source and include directory
set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src" CACHE INTERNAL "")
set(MUSHER_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include" CACHE INTERNAL "")

# Set variable for all source files within the source directory
set(SOURCES
    ${SOURCE_DIR}/functional_test.cpp
    ${SOURCE_DIR}/musher_wrapper.cpp)

set(TEST_SOURCES
    ${SOURCE_DIR}/functional_test.cpp
    ${SOURCE_DIR}/musher_test.cpp)

set(CMAKE_CXX_STANDARD 17)

# call build function within buildTools
musher_add_module(musher MODULE ${SOURCES})


# ################################
# # Testing
# ################################
# if (PACKAGE_TESTS)
#   # if (APPLE)
#   #   add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
#   #   add_definitions(-D__GLIBCXX__)
#   # endif (APPLE)

#   enable_testing()

#   # This adds another subdirectory, which has 'project(gtest)'.
#   add_subdirectory(lib/googletest)

#   # Include the gtest library. gtest_SOURCE_DIR is available due to
#   # 'project(gtest)' above.
#   include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

#   # ##############
#   # # Unit Tests
#   # ##############
#   # add_executable(runUnitTests test_project1.cpp)

#   # # Standard linking to gtest stuff.
#   # target_link_libraries(runUnitTests gtest gtest_main)

#   # # Extra linking for the project.
#   # target_link_libraries(runUnitTests project1_lib)

#   # # This is so you can do 'make test' to see all your tests run, instead of
#   # # manually running the executable runUnitTests to see those specific tests.
#   # add_test(NAME that-test-I-made COMMAND runUnitTests)

#   # # You can also omit NAME and COMMAND. The second argument could be some other
#   # # test executable.
#   # add_test(that-other-test-I-made runUnitTests)
# endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

# # https://cliutils.gitlab.io/modern-cmake/chapters/testing.html
# # Override for anyone wanting to enable testing on their package and this package.
# if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MYPROJECT_BUILD_TESTING) AND BUILD_TESTING)
#     add_subdirectory(tests)
# endif()
