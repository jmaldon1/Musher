import os
import sys
import platform
import subprocess
import shutil
import glob
import distutils.cmd

from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext

ROOT_DIR = os.path.dirname(os.path.abspath(__file__))

# # Various platform-dependent extras
# extra_compile_args = []
# extra_link_args = []

# # #349: something with OS X Mojave causes libstd not to be found
# if platform.system() == 'Darwin':
#     extra_compile_args += ['-std=c++11', '-mmacosx-version-min=10.9']
#     extra_link_args += ['-stdlib=libc++', '-mmacosx-version-min=10.9']

# file_path = os.path.abspath(os.path.dirname(__file__))
# song_project_path = os.path.abspath(os.path.join(file_path, ".."))
# include_path = os.path.join(song_project_path, "include")
# src_path = os.path.join(song_project_path, "src")

# # Definition of the extension modules
# # Compile from sources
# cSong = Extension("cSong",
#                 sources = [
#                     # os.path.join(src_path, "song_wrapper.cpp"),
#                     "song_wrapper.cpp",
#                     os.path.join(src_path, "song.cpp"),
#                     os.path.join(src_path, "functional_test.cpp")],
#                 include_dirs = [include_path],
#                 extra_compile_args=extra_compile_args,
#                 extra_link_args=extra_link_args)


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(["cmake", "--version"])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(
            os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(
                cfg.upper(),
                extdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get('CXXFLAGS', ''),
            self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args,
                              cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args,
                              cwd=self.build_temp)
        print()  # Add an empty line for cleaner output


class CleanBuildCommand(distutils.cmd.Command):
    user_options = []

    def initialize_options(self):
        pass

    def finalize_options(self):
        pass

    def run(self):
        # remove build folder
        build_path = os.path.join(ROOT_DIR, "build")

        try:
            shutil.rmtree(build_path)
        except OSError:
            pass

        # remove dist folder
        dist_path = os.path.join(ROOT_DIR, "dist")

        try:
            shutil.rmtree(dist_path)
        except OSError:
            pass

        # remove egg info folder
        egg_info_path = os.path.join(ROOT_DIR, "Musher.egg-info")

        try:
            shutil.rmtree(egg_info_path)
        except OSError:
            pass

        # remove shared libraries
        so_libs = glob.glob(os.path.join(ROOT_DIR, "*.so"))
        dlls_libs = glob.glob(os.path.join(ROOT_DIR, "*.dll"))
        dylibs_libs = glob.glob(os.path.join(ROOT_DIR, "*.dylib"))

        for so in so_libs:
            try:
                os.remove(so)
            except OSError:
                pass

        for dll in dlls_libs:
            try:
                os.remove(dll)
            except OSError:
                pass

        for dylib in dylibs_libs:
            try:
                os.remove(dylib)
            except OSError:
                pass
        print(u'\u2713', "cleaning done")


setup(
    name='Musher',
    version='0.1',
    author='Brian Silver, Joshua Maldonado',
    author_email='',
    description='A hybrid Python/C++ test project',
    long_description='',
    # add extension module
    ext_modules=[CMakeExtension("Musher", ROOT_DIR)],
    # add custom build_ext command
    cmdclass={"build_ext": CMakeBuild,
              "clean": CleanBuildCommand},
    zip_safe=False,
)
