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

# allows added submodules to find c++ header files
include_directories(${MUSHER_INCLUDE_DIR})

# Set variable for all source files within the source directory
# set(SOURCES
#     ${SOURCE_DIR}/functional_test.cpp
#     ${SOURCE_DIR}/musher_wrapper.cpp)

set(MUSHER_LIBRARY_SOURCES
    ${SOURCE_DIR}/functional_test.cpp)

set(MUSHER_SOURCES
    ${SOURCE_DIR}/musher_wrapper.cpp)

set(CMAKE_CXX_STANDARD 17)
set(MUSHER_LIB musher_library)



# ################################
# # Building
# ################################
musher_add_library(${MUSHER_LIB} SHARED ${MUSHER_LIBRARY_SOURCES})
# set_target_properties(${MUSHER_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE {PROJECT_SOURCE_DIR})
# if ( MSVC )
#     set_target_properties(${MUSHER_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
#     set_target_properties(${MUSHER_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR})
#     set_target_properties(${MUSHER_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR})
#     # etc for the other available configuration types (MinSizeRel, RelWithDebInfo)
# endif ( MSVC )
# set_target_properties(${MUSHER_LIB} PROPERTIES
#                       LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}$<0:>)
# add_custom_command(TARGET MUSHER_LIB POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         "${PROJECT_SOURCE_DIR}/build/$<CONFIGURATION>"
#         $<PROJECT_SOURCE_DIR:MUSHER_LIB>)

# windows dll export
add_definitions(-DMUSHER_EXPORT)


# Python library
# call build function within buildTools
musher_add_module(musher MODULE ${MUSHER_SOURCES})

# copy MUSHER_LIB output to Project root
# if (WIN32 OR CYGWIN)
    # file(GLOB MUSHER_DLLS ${CMAKE_BINARY_DIR}\\*.dll)
    # file(COPY ${MUSHER_DLLS} DESTINATION ${PROJECT_SOURCE_DIR})
# endif()

target_link_libraries(musher PRIVATE ${MUSHER_LIB})

# ################################
# # Testing
# ################################
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

# set(CONFIGURATION "Release")
# # if (MSVC)
# 	add_custom_command(TARGET test_musher_cpp POST_BUILD
# 	    COMMAND ${CMAKE_COMMAND} -E copy_directory
# 	        "${PROJECT_SOURCE_DIR}/$<CONFIGURATION>"
# 	        # $<PROJECT_SOURCE_DIR:test_musher_cpp>
# 	        ${PROJECT_SOURCE_DIR}
# 	        )
# endif()
# # https://cliutils.gitlab.io/modern-cmake/chapters/testing.html
# # Override for anyone wanting to enable testing on their package and this package.
# if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MYPROJECT_BUILD_TESTING) AND BUILD_TESTING)
#     add_subdirectory(tests)
# endif()
