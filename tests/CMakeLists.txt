cmake_minimum_required (VERSION 2.8.12)

# Prevent overriding the parent project's compiler/linker
# dll Settings on Windows
if (WIN32 OR CYGWIN)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

set(MUSHER_TEST_SOURCE_DIR "${PROJECT_SOURCE_DIR}/tests" CACHE INTERNAL "")
set(MUSHER_TEST_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/tests/test_include" CACHE INTERNAL "")

# define test source files
# set(MUSHER_CPP_TEST_SOURCES
#     "${MUSHER_TEST_SOURCE_DIR}/test_musher_library.cpp"
#     "${MUSHER_TEST_SOURCE_DIR}/test_musher_utils.cpp"
#     )

# # define test header files
set(MUSHER_CPP_TEST_INCLUDES
    "${MUSHER_LIBRARY_INLUDES}"
    "${MUSHER_TEST_INCLUDE_DIR}/test_load_audio_file.h"
    )

# allows added submodules to find c++ header files
# include_directories(${MUSHER_TEST_INCLUDE_DIR})

if(${CMAKE_VERSION} VERSION_LESS 3.11)
    # This requires that you pull all git submodules for this project
    add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")
else()
    include(FetchContent)

    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        v1.10.x
    )

    # macro providing same functionality as FetchContent_MakeAvailable (cmake 3.14+)
    # To support cmake version 3.11+
    if(${CMAKE_VERSION} VERSION_LESS 3.14)
        macro(FetchContent_MakeAvailable NAME)
            FetchContent_GetProperties(${NAME})
            if(NOT ${NAME}_POPULATED)
                FetchContent_Populate(${NAME})
                add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
            endif()
        endmacro()
    endif()

    FetchContent_MakeAvailable(googletest)
endif()

# keeps your CACHE cleaner
mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

# keeps IDEs that support folders clean
set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)

macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN}) 
    # target_link_libraries(${TESTNAME} gtest gmock gtest_main ${MUSHER_LIB})
    target_link_libraries(${TESTNAME} gtest gmock gtest_main musher_library)
    target_include_directories(${TESTNAME} PRIVATE ${MUSHER_LIBRARY_INCLUDE_DIR} ${MUSHER_TEST_INCLUDE_DIR})
    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
    set_target_properties(${TESTNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/test_bin)
    if (MSVC)
        set_target_properties(${TESTNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CONFIGURATION} ${PROJECT_SOURCE_DIR}/test_bin)
    endif()
    # set_target_properties(${TESTNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    # if (WIN32 OR CYGWIN OR MSVC)
    # add_custom_command(TARGET ${TESTNAME} POST_BUILD
    # COMMAND ${CMAKE_COMMAND} -E copy_directory
    #     "${PROJECT_SOURCE_DIR}/$<CONFIGURATION>"
    #     $<PROJECT_SOURCE_DIR:${TESTNAME}>)
	# endif()
    # set_target_properties(${TESTNAME} PROPERTIES FOLDER "tests/Release")
endmacro()

# package_add_test(test_musher_library "${MUSHER_TEST_SOURCE_DIR}/test_musher_library.cpp" "${MUSHER_CPP_TEST_INCLUDES}")
# package_add_test(test_musher_utils "${MUSHER_TEST_SOURCE_DIR}/test_musher_utils.cpp" "${MUSHER_CPP_TEST_INCLUDES}")


package_add_test(test_musher_library "${MUSHER_TEST_SOURCE_DIR}/test_musher_library.cpp" ${MUSHER_LIBRARY_SOURCES})
package_add_test(test_musher_utils "${MUSHER_TEST_SOURCE_DIR}/test_musher_utils.cpp" ${MUSHER_LIBRARY_SOURCES})
