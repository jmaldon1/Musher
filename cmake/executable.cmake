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
#   project_runner(NAME
#       SOURCES ...
#       DEPENDENCIES ...
#   )
macro(project_runner NAME)
    project_exe(${NAME}-runner ${ARGN})
    discover_tests(${NAME}-runner)
endmacro()