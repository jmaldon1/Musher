macro(setup)
    cmake_minimum_required(VERSION 3.5)
    cmake_policy(SET CMP0076 NEW) # target_sources() converts relative path to absolute
endmacro()

macro(add_project_dir_definitions target)
    target_compile_definitions(${target} PRIVATE
        SOURCE_DIR="${PROJECT_SOURCE_DIR}"
        BIN_DIR="${PROJECT_BINARY_DIR}"
        PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
endmacro()