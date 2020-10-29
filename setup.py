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


class CMakeBuild(build_ext):
    """
    Debug (with tests): python setup.py cmake --debug
    Release (no tests): python setup.py cmake
    """
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

        if self.debug:
            cmake_args = ["-DCMAKE_BUILD_TYPE=Debug", "-DENABLE_TESTS=On"]
        else:
            cmake_args = ["-DCMAKE_BUILD_TYPE=Release"]

        env = os.environ.copy()
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args,
                              cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'],
                              cwd=self.build_temp)


class CTest(test):
    """
    Run tests if compiled (only through DEBUG)
    
    python setup.py cmake_debug
    python setup.py ctest
    """
    def distutils_dir_name(self, dname):
        dir_name = "{dirname}.{platform}-{machine}-{version[0]}.{version[1]}"
        os = platform.system().lower()
        # fix windows path
        if os == "windows":
            os = "win"

        return dir_name.format(dirname=dname,
                               platform=os,
                               machine=platform.machine().lower(),
                               version=sys.version_info)      

    def run(self):
        cwd = os.path.join('build', self.distutils_dir_name('temp'))
        if platform.system().lower() == "windows":
            cwd = os.path.join(cwd, "Debug")
        subprocess.call(['ctest', "--output-on-failure"], cwd=cwd, shell=True)


setup(
    name='musher',
    version='0.1',
    description='Mush songs together',
    packages=find_packages(),
    ext_modules=[CMakeExtension("musher")],
    cmdclass={
        "clean": CleanBuildCommand,
        "cmake": CMakeBuild,
        "ctest": CTest,
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