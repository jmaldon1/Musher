
option(ENABLE_CONAN_INSTALL "Run conan install at start of cmake configure" ON)

if(ENABLE_CONAN_INSTALL)
    if (APPLE)
        execute_process(COMMAND conan install ${CMAKE_SOURCE_DIR} -s build_type=${CMAKE_BUILD_TYPE} -s compiler.libcxx=libc++ --build=gtest)
    elseif (UNIX OR MINGW)
        execute_process(COMMAND conan install ${CMAKE_SOURCE_DIR} -s build_type=${CMAKE_BUILD_TYPE} -s compiler.libcxx=libstdc++11)
    else()
        execute_process(COMMAND conan install ${CMAKE_SOURCE_DIR} -s build_type=${CMAKE_BUILD_TYPE})
    endif()
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)