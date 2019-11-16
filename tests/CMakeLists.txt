# Prevent overriding the parent project's compiler/linker
# dll Settings on Windows
if (WIN32 OR CYGWIN)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

# set(INSTALL_GTEST off)

# define test source files
set(MUSHER_CPP_TEST_SOURCES
    test_musher_cpp.cpp)

add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")

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
    target_link_libraries(${TESTNAME} gtest gmock gtest_main ${MUSHER_LIB})
    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
    set_target_properties(${TESTNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    # if (WIN32 OR CYGWIN OR MSVC)
    # add_custom_command(TARGET ${TESTNAME} POST_BUILD
    # COMMAND ${CMAKE_COMMAND} -E copy_directory
    #     "${PROJECT_SOURCE_DIR}/$<CONFIGURATION>"
    #     $<PROJECT_SOURCE_DIR:${TESTNAME}>)
	# endif()
    # set_target_properties(${TESTNAME} PROPERTIES FOLDER "tests/Release")
endmacro()

package_add_test(test_musher_cpp ${MUSHER_CPP_TEST_SOURCES})

# if (MSVC)
    # POSSIBLE FIX FOR MSVC
	# set(CONFIGURATION "Release")
	add_custom_command(TARGET test_musher_cpp POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy_directory
	        "${CMAKE_BINARY_DIR}/$<CONFIGURATION>"
	        # $<PROJECT_SOURCE_DIR:test_musher_cpp>
	        ${PROJECT_SOURCE_DIR}
	)
	message( ${CMAKE_BINARY_DIR} )
# endif()