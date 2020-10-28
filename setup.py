import os
import sys
import platform
import subprocess
import shutil
import glob
import codecs
import re
import ntpath

from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.test import test

# Fix error for importing distutils before setuptools
import distutils.cmd
from distutils.version import LooseVersion
from distutils import sysconfig

ROOT_DIR = os.path.dirname(os.path.abspath(__file__))

readme_note = """\
.. note::

   For the latest source, discussion, etc, please visit the
   `GitHub repository <https://github.com/jmaldon1/Musher>`_\n\n

"""

with codecs.open('README.md', encoding='utf-8') as fobj:
    long_description = readme_note + fobj.read()


class BuildCppTests(distutils.cmd.Command):
    description = 'Build c++ tests for musher library'
    user_options = [
        ('debug', None, 'sets config to Debug, config set to Release by default'),
        ('run-tests', 'r', 'set to run tests after building'),
        # https://github.com/google/googletest/blob/master/googletest/docs/advanced.md#running-a-subset-of-the-tests
        # Example: (NOTE: escape the * in ZSH to prevent globbing error.)
        #   python setup.py build_cpp_tests --r --g=--gtest_filter=HPCP.\*
        ('gtest-options=', 'g',
         'Any google test args that need to be passed through when building tests.'),
    ]

    def initialize_options(self):
        self.debug = False
        self.run_tests = False
        self.gtest_options = ""

    def finalize_options(self):
        pass

    def run(self):
        # extdir = os.path.abspath(
        #     os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = [f'-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={ROOT_DIR}']

        # Check if using g++ to compile
        if sysconfig.get_config_var('CXX') == "g++":
            # Check if user has g++ 8, if they do, use it to compile
            if os.path.exists("/usr/bin/g++-8"):
                cmake_args += ["-DCMAKE_CXX_COMPILER=/usr/bin/g++-8"]

        # Do not compile python module when building C++ code.
        cmake_args += ['-DBUILD_PYTHON_MODULE=OFF']

        if self.debug:
            cmake_args += ['-DCMAKE_BUILD_TYPE=Debug']

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += [
                f'-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{cfg.upper()}={ROOT_DIR}']
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        env = os.environ.copy()
        env['CXXFLAGS'] = " ".join([env.get("CXXFLAGS", ""),
                                    f'-DVERSION_INFO=\\"{self.distribution.get_version()}\\"'])
        build_dir = 'build/'
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)
        subprocess.check_call(['cmake', ROOT_DIR] + cmake_args,
                              cwd=build_dir, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args,
                              cwd=build_dir)
        print()  # Add an empty line for cleaner output

        if self.run_tests:
            print("running tests...")
            test_bin_dir = os.path.abspath("./test_bin")
            test_bins_glob = os.path.join(test_bin_dir, "*")
            test_bin_paths = [os.path.abspath(os.path.join(
                test_bin_dir, f)) for f in glob.glob(test_bins_glob)]
            for test_bin_path in test_bin_paths:
                test_name = ntpath.basename(test_bin_path)
                print("=" * 35)
                print(f"Running Test '{test_name}'")
                print("=" * 35)
                test_args = [test_bin_path]
                if self.gtest_options:
                    test_args += [self.gtest_options]
                subprocess.check_call(test_args, cwd=ROOT_DIR)


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

        if platform.system() == "Windows":
            cmake_version = LooseVersion(
                re.search(r'version\s*([\d.]+)', out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(
            os.path.dirname(self.get_ext_fullpath(ext.name)))
        # Output library to root directory
        cmake_args = [f'-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}',
                      f'-DPYTHON_EXECUTABLE={sys.executable}']

        # Check if using g++ to compile
        if sysconfig.get_config_var('CXX') == "g++":
            # Check if user has g++ 8, if they do, use it to compile
            if os.path.exists("/usr/bin/g++-8"):
                cmake_args += ["-DCMAKE_CXX_COMPILER=/usr/bin/g++-8"]

        # Do not build C++ tests when packaging code.
        cmake_args += ['-DBUILD_TESTING=OFF']

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            # Output windows library to root directory
            cmake_args += [
                f'-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{cfg.upper()}={extdir}']
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        env = os.environ.copy()
        env['CXXFLAGS'] = " ".join([env.get("CXXFLAGS", ""),
                                    f'-DVERSION_INFO=\\"{self.distribution.get_version()}\\"'])
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
        cleanup_list = [
            # Folders
            os.path.join(ROOT_DIR, "build"),
            os.path.join(ROOT_DIR, "dist"),
            os.path.join(ROOT_DIR, "musher.egg-info"),
            os.path.join(ROOT_DIR, ".eggs"),
            os.path.join(ROOT_DIR, ".pytest_cache"),
            os.path.join(ROOT_DIR, ".tox"),
            os.path.join(ROOT_DIR, "Release"),
            os.path.join(ROOT_DIR, "Debug"),
            os.path.join(ROOT_DIR, "test_bin"),
            # Files
            # clean up linux outputs
            *glob.glob(os.path.join(ROOT_DIR, "*.so")),
            *glob.glob(os.path.join(ROOT_DIR, "*.dylib")),
            # clean up windows outputs
            *glob.glob(os.path.join(ROOT_DIR, "*.pyd")),
            *glob.glob(os.path.join(ROOT_DIR, "*.dll")),
            *glob.glob(os.path.join(ROOT_DIR, "*.exe")),
            *glob.glob(os.path.join(ROOT_DIR, "*.exp")),
            *glob.glob(os.path.join(ROOT_DIR, "*.lib")),
        ]

        for item in cleanup_list:
            try:  # If item is a dir then remove it
                shutil.rmtree(item)
                print(f"cleaned {item}")
            except OSError:
                try:  # If item is a file then remove it
                    os.remove(item)
                    print(f"cleaned {item}")
                except OSError:
                    pass

        print(u'\u2713', "cleaning done")


class CMakeBuildDebug(build_ext):
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
        # Output library to root directory
        cmake_args = ["-DCMAKE_BUILD_TYPE=Debug", "-DENABLE_TESTS=On"]

        env = os.environ.copy()
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args,
                              cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'],
                              cwd=self.build_temp)


class CMakeBuildRelease(build_ext):
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
        # Output library to root directory
        cmake_args = ["-DCMAKE_BUILD_TYPE=Release"]

        env = os.environ.copy()
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args,
                              cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'],
                              cwd=self.build_temp)


# class CTest(test):
#     def distutils_dir_name(self, dname):
#         """Returns the name of a distutils build directory"""
#         dir_name = "{dirname}.{platform}-{version[0]}.{version[1]}"
#         return dir_name.format(dirname=dname,
#                                platform=sysconfig.get_platform(),
#                                version=sys.version_info)

#     def run(self):
#         subprocess.call(['ctest'],
#                         cwd=os.path.join('build',
#                                          self.distutils_dir_name('temp')),
#                         shell=True)


setup(
    name='musher',
    version='0.1',
    description='Mush songs together',
    packages=find_packages(),
    ext_modules=[CMakeExtension("musher")],
    cmdclass={
        "clean": CleanBuildCommand,
        "cmake_debug": CMakeBuildDebug,
        "cmake_release": CMakeBuildRelease,
        # "ctest": CTest,
        # "conan_debug":
        # conan_debug":
    },
    zip_safe=False,
    long_description=long_description,
    author='Brian Silver, Joshua Maldonado',
    author_email='joshjm9915@gmail.com',
    url='https://github.com/jmaldon1/Musher',
    license='MIT',
)
