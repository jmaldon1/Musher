include_guard(GLOBAL)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

# TODO: is this needed? fix  python
# For setting fPIC. Required for wavelib to compile.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(ENABLE_PACKAGE_BUILD "Build package using Conan" OFF)
option(ENABLE_TESTS "Build unit tests" OFF)

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    string(REGEX MATCH "release|debug" _match ${CMAKE_BINARY_DIR})
    if (CMAKE_MATCH_0 STREQUAL "")
        message(FATAL_ERROR "CMAKE_BUILD_TYPE not set. Please set, for example\ncmake .._DCMAKE_BUILD_TYPE=Debug\n")
    endif()
    message(STATUS "Setting CMAKE_BUILD_TYPE to ${CMAKE_MATCH_0}")
    set(CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

if (NOT APPLE AND (UNIX OR MINGW))
    option(FORCE_ABI "Force RHEL-style ABI" OFF)
    set(RELEASE_OPTIMIZATION_OPTION "-O2" CACHE STRING "Compiler optimization option for release builds")
    set(DEBUG_OPTIMIZATION_OPTION "-Og" CACHE STRING "Compiler optimization option for debug builds")

    if(FORCE_ABI)
        add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)
        add_link_options(-no-pie)
    endif()

    if(ENABLE_COVERAGE)
        set(EXTRA_CXX_FLAGS_DEBUG ${COVERAGE_COMPILER_FLAGS})
    endif()

    add_compile_options(
        -ggdb3
        -Wall
        -Wextra
        -Wshadow
        -Wno-unused-result
        -Wformat
        -Wformat-signedness
        -Werror
        -Wfatal-errors
        -Wno-buildin-macro-redefined
        -Wundef
    )

    set(_debug_options ${EXTRA_CXX_FLAGS_DEBUG} ${DEBUG_OPTIMIZATION_OPTION})
    set(_release_options ${RELEASE_OPTIMIZATION_OPTION})
    
    add_compile_options(
        "$<$<CONFIG:DEBUG>:${_debug_options}>"
        "$<$<CONFIG:RELEASE>:${_release_options}>")

    set(_debug_definitions _DEBUG DEBUG)
    set(_release_definitions NDEBUG)

    add_compile_definitions(
        "$<$<CONFIG:DEBUG>:${_debug_definitions}>"
        "$<$<CONFIG:RELEASE>:${_release_definitions}>")

elseif(WIN32)
    add_compile_options(/wd4267 /MP8 /Zc:__cplusplus /permissive- /Z7)
    add_compile_definitions(_WIN32_WINNT=0x0600 NOMINMAX)
endif()



if(NOT ENABLE_PACKAGE_BUILD AND ${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please run CMake from a build directory (e.g. cmake-build-debug)")
endif()

if (UNIX OR MINGW)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
endif()
set(CMAKE_HAVE_LIBC_PTHREAD OFF)
find_package(Threads)

include(GNUInstallDirs)

include(compile-options)
include(tests)
# include(python)
include(library)
# include(module)
include(executable)
include(utils)