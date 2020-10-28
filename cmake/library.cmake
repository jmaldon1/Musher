include(CMakeParseArguments)

# USAGE:
#   project_library(NAME
#       INTERFACE ...
#       SOURCES ...
#       DEPENDENCIES ...
#   )
macro(project_library NAME)
    setup()

    set(options OPTIONS)
    set(oneValueArgs)
    set(multiValueArgs INTERFACE SOURCES DEPENDENCIES)
    cmake_parse_arguments(project_library "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if(project_library_INTERFACE)
        add_library(${NAME} INTERFACE)
    else()
        add_library(${NAME})
        add_project_dir_definitions(${NAME})
    endif()

    if(project_library_INTERFACE)
        target_sources(${NAME} INTERFACE ${project_library_INTERFACE})
    endif()
    
    if(project_library_SOURCES)
        target_sources(${NAME} PRIVATE ${project_library_SOURCES})
    endif()

    if(project_library_INTERFACE)
        project_link_libraries(${NAME} INTERFACE ${project_library_INTERFACE})
    else()
        project_link_libraries(${NAME} PUBLIC ${project_library_DEPENDENCIES})
    endif()
    
    project_install(${NAME})
endmacro()


# USAGE:
#   project_link_libraries(NAME
#       INTERNAL ...
#       CONAN ...
#       OTHER ...
#   )
macro(project_link_libraries target op)
    set(options OPTIONS)
    set(oneValueArgs)
    set(multiValueArgs INTERNAL CONAN OTHER)
    cmake_parse_arguments(project_link_libraries "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    set(_INTERNAL ${project_link_libraries_INTERNAL})
    set(_CONAN ${project_link_libraries_CONAN})
    set(_OTHER ${project_link_libraries_OTHER})
    if(NOT _INTERNAL AND NOT _CONAN AND NOT _OTHER)
        set(_CONAN ${ARGN})
    endif()

    set(CONAN_LIBS)
    foreach(lib ${_CONAN})
        list(APPEND CONAN_LIBS "CONAN_PKG::${lib}")
    endforeach()

    target_link_libraries(${target} ${op}
        ${_INTERNAL}
        ${CONAN_LIBS}
        ${_OTHER})
endmacro()


# USAGE:
#   project_install_files(file.h ...)
macro(project_install_files)
    set(files ${ARGN})

    list(FILTER files INCLUDE REGEX "\\.(h|hpp)")
    message(DEBUG "Install headers: ${files}")

    file(RELATIVE_PATH _source_dir ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

    foreach(file ${files})
        get_filename_component(__dir ${file} DIRECTORY)
        message(DEBUG "Install ${file} => ${_dir} => ${CMAKE_INSTALL_INCLUDEDIR}/${_source_dir}/${_dir}")
        install(FILES ${file} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${_source_dir}/${_dir})
    endforeach()
endmacro()


# USAGE:
#   project_install(target)
macro(project_install target)
    install(TARGETS ${target}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIB_DIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    
        get_property(_has_interface_sources TARGET ${target} PROPERTY INTERFACE_SOURCES SET)
        if(_has_interface_sources)
            get_target_property(_sources ${target} INTERFACE_SOURCES)
        else()
            get_target_property(_sources ${target} SOURCES)
        endif()
        project_install_files(${_sources})
endmacro()