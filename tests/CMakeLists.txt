add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")

# include_directories(
# 	${MUSHER_INCLUDE_DIR}  # from project CMakeLists.txt
#   )
# include_directories(${MUSHER_INCLUDE_DIR})
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
    target_link_libraries(${TESTNAME} gtest gmock gtest_main)
    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()

package_add_test(test_musher_cpp
    ${PROJECT_SOURCE_DIR}/src/functional_test.cpp
    test_musher_cpp.cpp
    ${PROJECT_SOURCE_DIR}/include/functional_test.h
    ${PROJECT_SOURCE_DIR}/include/musher_core.h)