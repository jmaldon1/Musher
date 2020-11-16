cmake_minimum_required(VERSION 3.5)

if(WIN32 AND NOT "${CMAKE_SYSTEM_VERSION}")
    set(CMAKE_SYSTEM_VERSION 10.0)
endif()

project(musher CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include_directories(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src/third-party)

include(conan)
include(base)

add_subdirectory(src)

if(GENERATE_DOCS)
    add_subdirectory(docs)
endif()

