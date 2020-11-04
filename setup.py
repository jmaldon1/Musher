"""Setup that is responsible for building the C/C++ python extensions.
"""
import os
import platform
import subprocess
import shutil
import codecs
from setuptools import setup, find_packages, Extension, Command
from setuptools.command.build_ext import build_ext
from setuptools.command.test import test


README_NOTE = """\
.. note::

   For the latest source, discussion, etc, please visit the
   `GitHub repository <https://github.com/jmaldon1/Musher>`_\n\n

"""

with codecs.open('README.md', encoding='utf-8') as fobj:
    long_description = README_NOTE + fobj.read()


def get_build_dir() -> str:
    """Get the build directory that will store all C/C++ extension by-products.

    Returns:
        str: build directory path.
    """
    root_dir_path = os.path.dirname(os.path.realpath(__file__))
    return os.path.join(root_dir_path, "build")


# pylint: disable=too-few-public-methods
class CMakeExtension(Extension):
    """Project extension.
    """

    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        # self.sourcedir = os.path.abspath(sourcedir)
        self.sourcedir = os.path.abspath(sourcedir)


# pylint: disable=no-self-use
# pylint: disable=unnecessary-pass
# pylint: disable=attribute-defined-outside-init
class CleanBuildCommand(Command):
    """Clean the project directory of temporary files involved with
    building the C extension and python module.
    This will also uninstall the development version of musher.
    """
    user_options = []

    def initialize_options(self):
        """Initialize user options
        """
        pass

    def finalize_options(self):
        """Finalize user options
        """
        pass

    def run(self):
        """Run the clean
        """
        root_dir = os.path.dirname(os.path.abspath(__file__))

        cleanup_list = [
            # Cmake
            os.path.join(root_dir, "build"),
            # Python
            os.path.join(root_dir, "dist"),
            os.path.join(root_dir, "musher.egg-info"),
            os.path.join(root_dir, ".eggs"),
            os.path.join(root_dir, ".pytest_cache"),
            os.path.join(root_dir, ".tox"),
            os.path.join(root_dir, "musher", "musher_python.so")
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
        except OSError as err:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions)) from err

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        shared_lib_name = "musher_python.so"
        build_dir = get_build_dir()
        if self.debug:
            cmake_args = ["-DCMAKE_BUILD_TYPE=Debug", "-DENABLE_TESTS=On"]
        else:
            cmake_args = ["-DCMAKE_BUILD_TYPE=Release"]

        if not os.path.exists(build_dir):
            os.makedirs(build_dir)

        subprocess.run(['cmake', ext.sourcedir] + cmake_args,
                       cwd=build_dir,
                       check=True)
        subprocess.run(['cmake', '--build', '.'],
                       cwd=build_dir,
                       check=True)

        # Copy the python shared module to the musher dummy wrapper.
        shared_lib_path = os.path.join(build_dir, "lib", shared_lib_name)
        root_dir_path = os.path.dirname(os.path.realpath(__file__))
        python_module_path = os.path.join(root_dir_path, "musher", shared_lib_name)
        shutil.copyfile(shared_lib_path, python_module_path)


class CTest(test):
    """Run tests if compiled (only through DEBUG)

    python setup.py cmake --debug
    python setup.py ctest
    """

    def run(self):
        build_dir = get_build_dir()

        if platform.system().lower() == "windows":
            build_dir = os.path.join(build_dir, "Debug")

        try:
            result = subprocess.run(['ctest', "--output-on-failure"],
                                    cwd=build_dir,
                                    check=True,
                                    stderr=subprocess.PIPE)
        except FileNotFoundError:
            # cwd was not found.
            print("HINT: Did you compile the code first? "
                  "(python setup.py cmake --debug)")
            raise

        if result.stderr:
            print(result.stderr.decode("utf-8"))
            print("HINT: Did you compile the code in debug mode first? "
                  "(python setup.py cmake --debug)")
        if result.returncode == 8:
            print("C++ Seg fault.")


class GTest(test):
    r"""Run tests if compiled (only through DEBUG)

    python setup.py cmake --debug
    python setup.py gtest

    example using filters:
        python setup.py gtest --g=--gtest_filter=HPCP.\*

    Attributes:
        gtest_options (str): Any gtest options that should be used when running the tests.

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
        bin_dir = os.path.join(get_build_dir(), "bin")

        if platform.system().lower() == "windows":
            bin_dir = os.path.join(bin_dir, "Debug")

        try:
            result = subprocess.run(['./musher-core-test', self.gtest_options],
                                    cwd=bin_dir,
                                    check=True)
        except FileNotFoundError:
            # Test executable not found / cwd not found.
            print("HINT: Did you compile the code in debug mode first? "
                  "(python setup.py cmake --debug)")
            raise

        if result.returncode == -11:
            print("C++ Seg fault.")


setup(
    name='musher',
    version='0.1',
    description='Mush songs together',
    # packages=find_packages(),
    ext_modules=[CMakeExtension("musher")],
    cmdclass={
        # build_ext is called while running 'pip install .'
        "build_ext": CMakeBuild,
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
