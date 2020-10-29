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


def distutils_dir_name(dname: str) -> str:
    dir_name = "{dirname}.{platform}-{machine}-{version[0]}.{version[1]}"
    os = platform.system().lower()
    # fix windows path
    if os == "windows":
        os = "win"

    if os == "darwin":
        mac_ver, _, _ = platform.mac_ver()
        major_ver, minor_ver = tuple(mac_ver.split('.')[:2])
        os = f"macosx-{major_ver}.{minor_ver}"

    return dir_name.format(dirname=dname,
                           platform=os,
                           machine=platform.machine().lower(),
                           version=sys.version_info)


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CleanBuildCommand(distutils.cmd.Command):
    user_options = []

    def initialize_options(self):
        pass

    def finalize_options(self):
        pass

    def run(self):
        cleanup_list = [
            # Cmake
            os.path.join(ROOT_DIR, "build"),
            # Python
            os.path.join(ROOT_DIR, "dist"),
            os.path.join(ROOT_DIR, "musher.egg-info"),
            os.path.join(ROOT_DIR, ".eggs"),
            os.path.join(ROOT_DIR, ".pytest_cache"),
            os.path.join(ROOT_DIR, ".tox")
        ]

        for item in cleanup_list:
            try:  # If item is a dir then remove it
                shutil.rmtree(item)
                print(f"deleted {item}")
            except OSError:
                try:  # If item is a file then remove it
                    os.remove(item)
                    print(f"deleted {item}")
                except OSError:
                    pass

        print(u'\u2713', "cleaning done")


class CMakeBuild(build_ext):
    """
    Debug (with tests): python setup.py cmake --debug
    Release (no tests): python setup.py cmake
    """

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
        if self.debug:
            cmake_args = ["-DCMAKE_BUILD_TYPE=Debug", "-DENABLE_TESTS=On"]
        else:
            cmake_args = ["-DCMAKE_BUILD_TYPE=Release"]

        env = os.environ.copy()
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.run(['cmake', ext.sourcedir] + cmake_args,
                              cwd=self.build_temp, env=env)
        subprocess.run(['cmake', '--build', '.'],
                              cwd=self.build_temp)


class CTest(test):
    """
    Run tests if compiled (only through DEBUG)

    python setup.py cmake --debug
    python setup.py ctest
    """
    def run(self):
        cwd = os.path.join('build', distutils_dir_name('temp'))
        if platform.system().lower() == "windows":
            cwd = os.path.join(cwd, "Debug")
        result = subprocess.run(['ctest', "--output-on-failure"], cwd=cwd)
        if result.returncode == 8:
            print("C++ Seg fault.")


class GTest(distutils.cmd.Command):
    """
    Run tests if compiled (only through DEBUG)

    python setup.py cmake --debug
    python setup.py gtest

    example using filters:
        python setup.py gtest --g=--gtest_filter=HPCP.\*
    """
    description = 'Run google tests for c++ library'
    user_options = [
        ('gtest-options=', 'g',
         'Any google test args that need to be passed through when building tests.'),
    ]

    def initialize_options(self):
        self.gtest_options = ""

    def finalize_options(self):
        pass

    def run(self):
        bin_dir = os.path.join('build', distutils_dir_name('temp'), "bin")
        if platform.system().lower() == "windows":
            bin_dir = os.path.join(bin_dir, "Debug")
        result = subprocess.run(
            ['./musher-core-test', self.gtest_options], cwd=bin_dir)
        if result.returncode == -11:
            print("C++ Seg fault.")


setup(
    name='musher',
    version='0.1',
    description='Mush songs together',
    packages=find_packages(),
    ext_modules=[CMakeExtension("musher")],
    cmdclass={
        "cmake": CMakeBuild,
        "ctest": CTest,
        "gtest": GTest,
        "clean": CleanBuildCommand,
    },
    zip_safe=False,
    long_description=long_description,
    author='Brian Silver, Joshua Maldonado',
    author_email='joshjm9915@gmail.com',
    url='https://github.com/jmaldon1/Musher',
    license='MIT',
)
