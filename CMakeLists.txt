cmake_minimum_required (VERSION 3.5)

project(Musher)

# set (CMAKE_FIND_FRAMEWORK NEVER)

# set(HELLO "HI")
# message(${HELLO})

# set(PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} CACHE INTERNAL "")
# set(PYTHON_LIBRARIES ${PYTHON_LIBRARIES} CACHE INTERNAL "")
# set(PYTHON_MODULE_PREFIX ${PYTHON_MODULE_PREFIX} CACHE INTERNAL "")
# set(PYTHON_MODULE_EXTENSION ${PYTHON_MODULE_EXTENSION} CACHE INTERNAL "")
# set(PYTHON_VERSION_MAJOR ${PYTHON_VERSION_MAJOR} CACHE INTERNAL "")
# set(PYTHON_VERSION_MINOR ${PYTHON_VERSION_MINOR} CACHE INTERNAL "")


# message(${PYTHON_INCLUDE_DIRS})

# set(FINDPYTHONLIBSNEW )
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/tools")
find_package(PythonLibsNew REQUIRED)

# find_package(Python3)
# find_package (Python COMPONENTS Interpreter)
# find_package(PythonLibs REQUIRED)
# set(PYBIND11_CMAKECONFIG_INSTALL_DIR "share/cmake/${PROJECT_NAME}" CACHE STRING "install path for pybind11Config.cmake")
# install(FILES tools/FindPythonLibsNew.cmake
#           DESTINATION "test/")
# find_package_message

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

# install(FILES ./FindPythonLibsNew.cmake)

# Remove prefix from generated .so file
SET_TARGET_PROPERTIES(Musher PROPERTIES PREFIX "")