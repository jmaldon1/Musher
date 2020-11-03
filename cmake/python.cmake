find_package(PythonInterp 3.6 REQUIRED)

# Find Python headers
exec_program(${PYTHON_EXECUTABLE}
    ARGS "-c \"import sysconfig; print(sysconfig.get_paths()['include'])\""
    OUTPUT_VARIABLE PYTHON_INCLUDE_DIRS
    RETURN_VALUE PYTHON_INCLUDE_DIRS_NOT_FOUND
    )
if(PYTHON_INCLUDE_DIRS_NOT_FOUND)
    message(FATAL_ERROR "Python headers not found")
endif()

# This goes after, since it uses PythonInterp as a hint
if(WIN32)
    find_package(PythonLibs 3.6 REQUIRED)
endif()


# USAGE:
#   project_py_module(NAME
#       SOURCES ...
#       DEPENDENCIES ...
#   )
macro(project_py_module NAME)
    setup()

    set(options OPTIONS)
    set(oneValueArgs)
    set(multiValueArgs SOURCES DEPENDENCIES)
    cmake_parse_arguments(project_library "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    add_library(${NAME} SHARED ${PYTHON_C_EXTENSION_SRCS})

    if(project_library_SOURCES)
        target_sources(${NAME} PRIVATE ${project_library_SOURCES})
    endif()

    set_target_properties(${NAME} PROPERTIES PREFIX "")

    if(WIN32)
        set_target_properties(
            ${NAME}
            PROPERTIES
            SUFFIX ".pyd"
        )
    endif()

    target_include_directories(${NAME} PUBLIC ${PYTHON_INCLUDE_DIRS} )

    # On Windows, it is required to link to the Python libraries
    if(WIN32)
        project_link_libraries(${NAME} PUBLIC ${PYTHON_LIBRARIES})
        project_link_libraries(${NAME} PUBLIC ${PYTHON_LIBRARIES} ${project_library_DEPENDENCIES})
    else()
        project_link_libraries(${NAME} PUBLIC ${project_library_DEPENDENCIES})
    endif()
endmacro()