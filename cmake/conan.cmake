
option(ENABLE_CONAN_INSTALL "Run conan install at start of cmake configure" ON)

if(ENABLE_CONAN_INSTALL)
    execute_process(COMMAND conan install ${CMAKE_SOURCE_DIR} -s build_type=${CMAKE_BUILD_TYPE})
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)