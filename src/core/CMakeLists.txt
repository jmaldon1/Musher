project_library(musher-core
    SOURCES
        utils.h
        utils.cpp
        key.h
        key.cpp
        musher_library.h
        musher_library.cpp
    DEPENDENCIES
        # CONAN
        #     functionalplus
        # OTHER
        #     wavelib
)


# TODO: Go through code and fix warnings
relaxed_compile_options(musher-core PUBLIC)

if(ENABLE_TESTS)
    add_subdirectory(test)
endif()
