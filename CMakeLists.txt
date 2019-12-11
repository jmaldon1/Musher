cmake_minimum_required (VERSION 2.8.12)

if (POLICY CMP0048)
	cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

# use g++-8 for gnu 
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(CMAKE_CXX_COMPILER "/usr/bin/g++-8")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(musher LANGUAGES CXX)

# Turn off with 'cmake -DBUILD_PYTHON_MODULE=OFF'
# Build code without python module, useful for building only tests
option(BUILD_PYTHON_MODULE "Build all tests." ON)

# set(CMAKE_VERBOSE_MAKEFILE ON)

# Add /tools to cmake path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/tools")

# Include buildTools cmake
include(buildTools)

# Include useful testing variables, only when this is the main CMakeList
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

if(APPLE)
	# This requires your mac to be updated to Catalina 
	# or else there will be a compilation error trying to find C++ libs
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)
endif()

set(MUSHER_LIBRARY_DIR "${CMAKE_CURRENT_LIST_DIR}/core/library")
set(MUSHER_PY_WRAPPER_DIR "${CMAKE_CURRENT_LIST_DIR}/core/py_wrapper")

set(MUSHER_LIBRARY_INCLUDE_DIR "${MUSHER_LIBRARY_DIR}/include")
set(MUSHER_LIBRARY_SOURCE_DIR "${MUSHER_LIBRARY_DIR}")

set(MUSHER_PY_WRAPPER_INCLUDE_DIR "${MUSHER_PY_WRAPPER_DIR}/include")
set(MUSHER_PY_WRAPPER_SOURCE_DIR "${MUSHER_PY_WRAPPER_DIR}")


# ################################
# # Building
# ################################
add_subdirectory("${PROJECT_SOURCE_DIR}/dependencies/wavelib" "dependencies/wavelib")
add_subdirectory(core/library)

if(BUILD_PYTHON_MODULE)
	add_subdirectory(core/py_wrapper)

endif()
# ################################
# # Testing
# ################################
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
