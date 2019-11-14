import os
import sys
import platform
import subprocess
import shutil
import glob
import distutils.cmd
import codecs
from functools import reduce

from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.test import test as TestCommand

ROOT_DIR = os.path.dirname(os.path.abspath(__file__))

readme_note = """\
.. note::

   For the latest source, discussion, etc, please visit the
   `GitHub repository <https://github.com/jmaldon1/Musher>`_\n\n

"""

with codecs.open('README.md', encoding='utf-8') as fobj:
    long_description = fobj.read()

tests_require = [
    'pytest',
]

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


class PyTest(TestCommand):
    user_options = [('pytest-args=', 'a', "Arguments to pass to py.test")]

    def initialize_options(self):
        TestCommand.initialize_options(self)
        self.pytest_args = []

    def finalize_options(self):
        TestCommand.finalize_options(self)
        self.test_args = []
        self.test_suite = True

    def run_tests(self):
        # import here, cause outside the eggs aren't loaded
        import pytest
        errno = pytest.main(self.pytest_args)
        sys.exit(errno)


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            subprocess.check_output(["cmake", "--version"])
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
        cleanup_dir_list = [
            os.path.join(ROOT_DIR, "build"),
            os.path.join(ROOT_DIR, "dist"),
            os.path.join(ROOT_DIR, "Musher.egg-info"),
            os.path.join(ROOT_DIR, ".eggs"),
            os.path.join(ROOT_DIR, ".pytest_cache"),
            *glob.glob(os.path.join(ROOT_DIR, "*.so")),  # Spread any lists
            *glob.glob(os.path.join(ROOT_DIR, "*.dll")),  # Spread any lists
            *glob.glob(os.path.join(ROOT_DIR, "*.dylib")),  # Spread any lists
            *glob.glob(os.path.join(ROOT_DIR, "*.pyd")) # clean up windows outputs 
        ]

        for item in cleanup_dir_list:
            try:  # If item is a dir then remove it
                shutil.rmtree(item)
            except OSError:
                try:  # If item is a file then remove it
                    os.remove(item)
                except OSError:
                    pass

        print(u'\u2713', "cleaning done")


setup(
    name='Musher',
    version='0.1',
    description='A hybrid Python/C++ test project',
    packages=find_packages(),
    # add extension module
    ext_modules=[CMakeExtension("Musher")],
    # add custom build_ext command
    cmdclass={"build_ext": CMakeBuild,
              "clean": CleanBuildCommand,
              "test": PyTest
              },
    zip_safe=False,
    long_description=long_description,
    author='Brian Silver, Joshua Maldonado',
    author_email='joshjm9915@gmail.com',
    url='https://github.com/jmaldon1/Musher',
    license='MIT',
    tests_require=tests_require,
)
