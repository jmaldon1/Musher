cmake_minimum_required (VERSION 2.8.12)

if (POLICY CMP0048)
	cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(musher LANGUAGES CXX)

# Turn off with 'cmake -DBUILD_PYTHON_MODULE=OFF'
# Build code without python module, useful for building only tests
option(BUILD_PYTHON_MODULE "Build all tests." ON)

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

# allows added submodules to find c++ header files
include_directories(${MUSHER_INCLUDE_DIR})

if(APPLE)
	# This requires your mac to be updated to Catalina 
	# or else there will be a compilation error trying to find C++ libs
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)
endif()

set(CMAKE_CXX_STANDARD 17)

set(MUSHER_LIBRARY_SOURCES
    ${SOURCE_DIR}/functional_test.cpp)

set(MUSHER_WRAPPER
    ${SOURCE_DIR}/musher_wrapper.cpp)

set(MUSHER_LIB musher_library)

# ################################
# # Building
# ################################
# musher_library needs to be STATIC (link during build time) for it to link with other libraries correctly
# If not set to STATIC, python will have a runtime linking error when pip installing then importing musher
# Atleast for MacOSX and Windows
if (APPLE OR WIN32)
	set(LIB_TYPE STATIC)
else()
	set(LIB_TYPE SHARED)
endif()

musher_add_library(${MUSHER_LIB} ${LIB_TYPE} ${MUSHER_LIBRARY_SOURCES})

# windows dll export
add_definitions(-DMUSHER_EXPORT)


if(BUILD_PYTHON_MODULE)
	# Python module
	# Call build function within buildTools
	musher_add_module(musher MODULE ${MUSHER_WRAPPER})

	# Link library to python module
	target_link_libraries(musher PRIVATE ${MUSHER_LIB})

endif()

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
