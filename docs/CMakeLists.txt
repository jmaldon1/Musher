find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

# Find all headers for this project
file(GLOB_RECURSE PROJECT_HEADERS ${CMAKE_SOURCE_DIR}/src/*.h)
# Find project python module
file(GLOB_RECURSE PROJECT_PYTHON_SO ${CMAKE_SOURCE_DIR}/musher/*.so)

if (NOT EXISTS "${PROJECT_PYTHON_SO}")
    MESSAGE(FATAL_ERROR "Python module is required to generate documentation. Consider using `pip install -e .`")
endif()

# == Doxygen ==
set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/src/core)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# Doxygen won't create this for us
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

# Only regenerate Doxygen when:
# - Header files have been updated
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                   DEPENDS ${PROJECT_HEADERS}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                   COMMENT "Generating documentation with Doxygen"
                   VERBATIM)

# Nice named target so we can run the job easily
add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

# == Sphinx ==
set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)
# Find Documentation source files
file(GLOB_RECURSE DOC_SOURCE_FILES ${SPHINX_SOURCE}/*.rst)

# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated
add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                   COMMAND 
                    ${SPHINX_EXECUTABLE} -b html
                    # Tell Breathe where to find the Doxygen output
                    -Dbreathe_projects.musher=${DOXYGEN_OUTPUT_DIR}/xml
                    ${SPHINX_SOURCE} ${SPHINX_BUILD}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS
                    ${DOC_SOURCE_FILES}
                    ${DOXYGEN_INDEX_FILE}
                   MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                   COMMENT "Generating documentation with Sphinx")

# Nice named target so we can run the job easily
add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})

# Add an install target to install the docs
install(DIRECTORY ${SPHINX_BUILD}
DESTINATION ${CMAKE_INSTALL_DOCDIR})
