cmake_minimum_required (VERSION 2.8.12)

if (POLICY CMP0048)
	cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

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

# use g++-8 for gnu 
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(CMAKE_CXX_COMPILER "/usr/bin/g++-8")
endif()

set(CMAKE_CXX_STANDARD 17)

# Set variable for source and include directory
set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src" CACHE INTERNAL "")
set(MUSHER_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include" CACHE INTERNAL "")

# SET(MUSHER_LIBRARY_INLUDES
# 	"${MUSHER_INCLUDE_DIR}/musher_core.h"
# 	"${MUSHER_INCLUDE_DIR}/musher_library.h"
# 	"${MUSHER_INCLUDE_DIR}/utils.h"
# 	)

# set(MUSHER_LIBRARY_SOURCES
#     "${SOURCE_DIR}/musher_library.cpp"
#     "${SOURCE_DIR}/utils.cpp"
#     )

# set(MUSHER_WRAPPER_INCLUDES
# 	"${MUSHER_INCLUDE_DIR}/python_utils.h"
# 	)

set(MUSHER_WRAPPER_SOURCES
    "${SOURCE_DIR}/musher_wrapper.cpp"
    "${SOURCE_DIR}/python_utils.cpp"
	)
	
set(MUSHER_LIBRARY_SOURCES
    "${SOURCE_DIR}/utils.cpp"
    "${SOURCE_DIR}/musher_library.cpp"
	)

# ################################
# # Building
# ################################
# musher_library needs to be STATIC (link during build time) for it to link with other libraries correctly
# If not set to STATIC, python will have a runtime linking error when pip installing then importing musher
# Atleast for MacOSX and Windows
# if (APPLE OR WIN32)
# 	set(LIB_TYPE STATIC)
# else()
# 	set(LIB_TYPE SHARED)
# endif()

# musher_add_library(${MUSHER_LIB} ${LIB_TYPE} ${MUSHER_LIBRARY_SOURCES} ${MUSHER_LIBRARY_INLUDES})
# target_include_directories(${MUSHER_LIB} PRIVATE ${MUSHER_INCLUDE_DIR})

# fix GNU filesystem c++17 linking error
# if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	# target_link_libraries(${MUSHER_LIB} PRIVATE stdc++fs)
# endif()

# windows dll export
# add_definitions(-DMUSHER_EXPORT)


if(BUILD_PYTHON_MODULE)
	# Python module
	# Call build function within buildTools
	# musher_add_module(musher MODULE ${MUSHER_WRAPPER_SOURCES} ${MUSHER_LIBRARY_INLUDES} ${MUSHER_WRAPPER_INCLUDES})
	musher_add_module(musher MODULE ${MUSHER_LIBRARY_SOURCES} ${MUSHER_WRAPPER_SOURCES})
	# Allow module to find includes
	target_include_directories(musher PRIVATE ${MUSHER_INCLUDE_DIR})

	# Link library to python module
	# target_link_libraries(musher PRIVATE ${MUSHER_LIB})

	# fix GNU filesystem c++17 linking error
	if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
		target_link_libraries(musher PRIVATE stdc++fs)
	endif()

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