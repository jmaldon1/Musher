include(CMakeParseArguments)


# USAGE:
#   project_exe(NAME
#       SOURCES ...
#       DEPENDENCIES ...
#   )
macro(project_exe NAME)
    setup()

    set(options OPTIONS)
    set(oneValueArgs)
    set(multiValueArgs SOURCES DEPENDENCIES)
    cmake_parse_arguments(project_exe "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${NAME})

    add_project_dir_definitions(${NAME})

    if(project_exe_SOURCES)
        target_sources(${NAME} PRIVATE ${project_exe_SOURCES})
    endif()

    project_link_libraries(${NAME} PUBLIC ${project_exe_DEPENDENCIES})
    project_install(${NAME})
endmacro()


# USAGE:
#   project_test(NAME
#       SOURCES ...
#       DEPENDENCIES ...
#   )
macro(project_test NAME)
    project_exe(${NAME} ${ARGN})
    # add_project_test(${NAME})
    discover_tests(${NAME})
endmacro()