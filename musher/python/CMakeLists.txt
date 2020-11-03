# # Python module

# set(MUSHER_PY_WRAPPER_SOURCES
#     "${MUSHER_PY_WRAPPER_SOURCE_DIR}/musher_wrapper.cpp"
#     "${MUSHER_PY_WRAPPER_SOURCE_DIR}/python_utils.cpp"
# )

# # Call build function within buildTools
# # musher_add_module(musher MODULE ${MUSHER_WRAPPER_SOURCES} ${MUSHER_LIBRARY_INLUDES} ${MUSHER_WRAPPER_INCLUDES})
# musher_add_module(musher MODULE ${MUSHER_PY_WRAPPER_SOURCES})
# # Allow module to find includes
# target_include_directories(musher PRIVATE ${MUSHER_PY_WRAPPER_INCLUDE_DIR} ${MUSHER_LIBRARY_INCLUDE_DIR})

# # Link dependencies
# target_link_libraries(musher PRIVATE musher_library wavelib)

# # fix GNU filesystem c++17 linking error
# if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
#     target_link_libraries(musher PRIVATE stdc++fs)
# endif()

project_py_module(musher-python
    SOURCES
        # utils.h
        # utils.cpp
        wrapper.cpp
    DEPENDENCIES
        INTERNAL
            musher-core
)
