cmake_minimum_required (VERSION 2.8.12)

project(musher)

# Add /tools to cmake path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/tools")

# Include buildTools cmake
include(buildTools)

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

# call build function within buildTools
musher_add_module(musher MODULE ${SOURCES})
