cmake_minimum_required (VERSION 2.8.12)

if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(musher LANGUAGES CXX)

# Options. Turn on with 'cmake -Dmyvarname=ON'.
# option(PACKAGE_TESTS "Build the tests" ON) # Makes boolean 'test' available.

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

include_directories(${MUSHER_INCLUDE_DIR})

# Set variable for all source files within the source directory
set(SOURCES
    ${SOURCE_DIR}/functional_test.cpp
    ${SOURCE_DIR}/musher_wrapper.cpp)

set(CMAKE_CXX_STANDARD 17)

# call build function within buildTools
musher_add_module(musher MODULE ${SOURCES})

# ################################
# # Testing
# ################################
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

# # https://cliutils.gitlab.io/modern-cmake/chapters/testing.html
# # Override for anyone wanting to enable testing on their package and this package.
# if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MYPROJECT_BUILD_TESTING) AND BUILD_TESTING)
#     add_subdirectory(tests)
# endif()
