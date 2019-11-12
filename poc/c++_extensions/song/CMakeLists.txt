cmake_minimum_required(VERSION 3.5)

project(song)

############################################################
# Create a library
############################################################

# Generate the shared library from the library sources
add_library(song_library SHARED 
    src/song.cpp
    src/functional_test.cpp
)

target_include_directories(song_library
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

add_definitions(-DSONG_EXPORT)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(song_test 
    src/song_test.cpp
)

# link the song_library target with the song_test target
target_link_libraries(song_test
    PRIVATE 
        song_library
)

add_definitions(-DSONG_IMPORT)