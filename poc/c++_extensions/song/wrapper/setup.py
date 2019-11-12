import os, sys

try:
    from distutils.core import setup, Extension
except:
    raise RuntimeError("\n\nPython distutils not found\n")

file_path = os.path.abspath(os.path.dirname(__file__))
song_project_path = os.path.abspath(os.path.join(file_path, ".."))
include_path = os.path.join(song_project_path, "include")
src_path = os.path.join(song_project_path, "src")

# Definition of the extension modules
# Compile from sources
cSong = Extension("cSong",
                sources = [
                    # os.path.join(src_path, "song_wrapper.cpp"),
                    "song_wrapper.cpp",
                    os.path.join(src_path, "song.cpp"),
                    os.path.join(src_path, "functional_test.cpp")],
                include_dirs = [include_path])

# Compile from shared library
# Does not currently work
# cSong = Extension("cSong",
#                 sources = ["song_wrapper.cpp"],
#                 include_dirs = [os.path.join("..", "include")],
#                 define_macros=[("SONG_IMPORT", '1')],
#                 runtime_library_dirs=[os.path.abspath(os.path.dirname(os.path.relpath(__file__)))],
#                 library_dirs=[os.path.abspath(os.path.dirname(os.path.relpath(__file__)))],
#                 # library_dirs=[os.path.join("..", "..", "build", "linux", "debug", "song")],
#                 # runtime_library_dirs=[os.path.join("..", "..", "build", "linux", "debug", "song")],
#                 libraries=["libsong_library"])


# Compile Python module
setup (ext_modules = [cSong],
        name = "cSong",
        description = "cSong Python module",
        version = "1.0")