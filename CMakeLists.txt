cmake_minimum_required (VERSION 3.5)

project(Musher)

find_package(PythonLibs REQUIRED)

set(CMAKE_SHARED_LIBRARY_PREFIX "")
# set(CMAKE_MODULE_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")

set(SOURCE_DIR "src")
include_directories(
    "include"
    ${PYTHON_INCLUDE_DIRS})

set(SOURCES
    ${SOURCE_DIR}/functional_test.cpp
    ${SOURCE_DIR}/musher_wrapper.cpp)

set(TEST_SOURCES
    ${SOURCE_DIR}/functional_test.cpp
    ${SOURCE_DIR}/musher_test.cpp)

add_library(Musher MODULE ${SOURCES})
target_link_libraries(Musher ${PYTHON_LIBRARIES})
add_definitions(-DMUSHER_EXPORT)

SET_TARGET_PROPERTIES(Musher PROPERTIES PREFIX "")