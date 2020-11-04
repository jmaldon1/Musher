project_library(musher-core
    SOURCES
        utils.h
        utils.cpp
        key.h
        key.cpp
        musher_library.h
        musher_library.cpp
    DEPENDENCIES
        CONAN
            functionalplus
        OTHER
            wavelib
)

if(ENABLE_TESTS)
    add_subdirectory(test)
endif()
