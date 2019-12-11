set(MUSHER_LIBRARY_SOURCES
    "${MUSHER_LIBRARY_SOURCE_DIR}/utils.cpp"
    "${MUSHER_LIBRARY_SOURCE_DIR}/musher_library.cpp"
    "${MUSHER_LIBRARY_SOURCE_DIR}/FftComplex.cpp"
)

if (APPLE OR WIN32)
	set(LIB_TYPE STATIC)
else()
	set(LIB_TYPE SHARED)
endif()

musher_add_library(musher_library ${LIB_TYPE} ${MUSHER_LIBRARY_SOURCES})
target_include_directories(musher_library PRIVATE ${MUSHER_LIBRARY_INCLUDE_DIR})
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_libraries(musher_library PRIVATE stdc++fs)
endif()