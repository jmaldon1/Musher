include(CMakeParseArguments)

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

    add_library(${NAME} MODULE)
    add_project_dir_definitions(${NAME})

    if(project_library_SOURCES)
        target_sources(${NAME} PRIVATE ${project_library_SOURCES})
    endif()

    # add Python includes
    target_include_directories(${NAME} PRIVATE ${PYTHON_INCLUDE_DIRS})

    if(PYTHON_IS_DEBUG)
        target_compile_definitions(${NAME} PRIVATE Py_DEBUG)
    endif()

    set_target_properties(${NAME} PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}")
    set_target_properties(${NAME} PROPERTIES SUFFIX "${PYTHON_MODULE_EXTENSION}")

    if(WIN32 OR CYGWIN)
        # Link against the Python shared library on Windows
        target_link_libraries(${NAME} PRIVATE ${PYTHON_LIBRARIES})
    elseif(APPLE)
        # It's quite common to have multiple copies of the same Python version
        # installed on one's system. E.g.: one copy from the OS and another copy
        # that's statically linked into an application like Blender or Maya.
        # If we link our plugin library against the OS Python here and import it
        # into Blender or Maya later on, this will cause segfaults when multiple
        # conflicting Python instances are active at the same time (even when they
        # are of the same version).

        # Windows is not affected by this issue since it handles DLL imports
        # differently. The solution for Linux and Mac OS is simple: we just don't
        # link against the Python library. The resulting shared library will have
        # missing symbols, but that's perfectly fine -- they will be resolved at
        # import time.

        target_link_libraries(${NAME} PRIVATE "-undefined dynamic_lookup")

    endif()

    project_link_libraries(${NAME} PUBLIC ${project_library_DEPENDENCIES})

    
    # project_install(${NAME})
endmacro()