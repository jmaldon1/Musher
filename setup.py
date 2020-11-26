"""Setup that is responsible for building the C++ python extensions.
"""
import os
import platform
import subprocess
import shutil
import glob
import signal
from typing import Callable
from setuptools import setup, find_packages, Extension, Command
from setuptools.command.test import test

import pybind11


ROOT_DIR = os.path.dirname(os.path.realpath(__file__))


def get_build_dir() -> str:
    """Get the build directory that will store all C/C++ extension by-products.

    Returns:
        str: build directory path.
    """
    return os.path.join(ROOT_DIR, "build")


# pylint: disable=no-self-use
# pylint: disable=unnecessary-pass
# pylint: disable=attribute-defined-outside-init
class CleanProject(Command):
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

        cleanup_list = [
            # Cmake
            os.path.join(ROOT_DIR, "build"),
            # Python
            os.path.join(ROOT_DIR, "dist"),
            os.path.join(ROOT_DIR, "musher.egg-info"),
            os.path.join(ROOT_DIR, ".eggs"),
            os.path.join(ROOT_DIR, ".pytest_cache"),
            os.path.join(ROOT_DIR, ".tox"),
            *glob.glob(os.path.join(ROOT_DIR, "musher", "*.so")),
            # Doxygen
            os.path.join(ROOT_DIR, "docs", "html"),
            os.path.join(ROOT_DIR, "docs", "latex"),
            os.path.join(ROOT_DIR, "docs", "xml"),
            # Sphinx
            os.path.join(ROOT_DIR, "docs", "build")
        ]

        for item in cleanup_list:
            try:  # If item is a dir then remove it
                shutil.rmtree(item)
                print("deleted {}".format(item))
            except OSError:
                try:  # If item is a file then remove it
                    os.remove(item)
                    print("deleted {}".format(item))
                except OSError:
                    pass

        print(u'\u2713', "cleaning done")


class PublishDocs(Command):
    """Publish the generated documentation to Github pages

        You must install the python module and generate the docs BEFORE running this command.
            1. pip install -e .
            2. python setup.py cmake --docs

        Example usage:
            python setup.py publish_docs -m "Added docs for new function"

        Helpful reads:
            https://discourse.gohugo.io/t/simple-deployment-to-gh-pages/5003
    """
    user_options = [
        ('message=', 'm', "Commit message when publishing docs."),
    ]

    def initialize_options(self):
        """Initialize user options
        """
        self.message = "Updated docs"

    def finalize_options(self):
        """Finalize user options
        """
        pass

    def copytree(self, src: str, dst: str, ignore: Callable = None):
        """copytree for older python versions that don't have the latest
        version of shutil.copytree(dirs_exist_ok=True)
        """
        if os.path.isdir(src):
            if not os.path.isdir(dst):
                os.makedirs(dst)
            files = os.listdir(src)
            if ignore is not None:
                ignored = ignore(src, files)
            else:
                ignored = set()
            for _file in files:
                if _file not in ignored:
                    self.copytree(os.path.join(src, _file),
                                  os.path.join(dst, _file),
                                  ignore)
        else:
            shutil.copyfile(src, dst)

    def run(self):
        """Publish to github pages
        """

        sphinx_build_dir = os.path.join(ROOT_DIR, 'build', 'docs', 'sphinx')
        temp_gh_pages_dir = os.path.join(ROOT_DIR, 'temp_gh_pages')
        git_worktree = os.path.join(
            ROOT_DIR, '.git', 'worktrees', "temp_gh_pages")
        nojekyll = os.path.join(temp_gh_pages_dir, ".nojekyll")

        # Cleanup to ensure we are starting fresh.
        try:
            shutil.rmtree(temp_gh_pages_dir)
        except OSError:
            pass
        subprocess.run(['git', 'worktree', 'prune'], cwd=ROOT_DIR, check=True)
        try:
            shutil.rmtree(git_worktree)
        except OSError:
            pass

        print("\nAdding gh-pages branch as a worktree...")
        subprocess.run(['git', 'worktree', 'add', '-B', 'gh-pages',
                        temp_gh_pages_dir, 'origin/gh-pages'], cwd=ROOT_DIR, check=True)
        # Pull to ensure we are up to date.
        subprocess.run(['git', 'pull'], cwd=temp_gh_pages_dir, check=True)

        # Copy newly generated docs
        print("\nCopying newly generated docs to git branch...")
        try:
            shutil.copytree(sphinx_build_dir, temp_gh_pages_dir,
                            dirs_exist_ok=True)
        except TypeError:
            # Not using python3.8+
            self.copytree(sphinx_build_dir, temp_gh_pages_dir)

        # Add a .nojekyll files to ensure gh-pages uses index.html as the root of the site.
        open(nojekyll, 'w').close()

        subprocess.run(['git', 'add', '--all'],
                       cwd=temp_gh_pages_dir, check=True)
        try:
            print("\nCommitting changes...")
            subprocess.run(['git', 'commit', '-m', self.message],
                           cwd=temp_gh_pages_dir, check=True)
            print("\nPushing to github...")
            subprocess.run(['git', 'push', 'origin', 'gh-pages'],
                           cwd=temp_gh_pages_dir, check=True)
        except subprocess.CalledProcessError as err:
            if err.returncode != 1:
                # returncode == 1 means the branch is up to date.
                raise err

        shutil.rmtree(temp_gh_pages_dir)

        subprocess.run(['git', 'worktree', 'prune'], cwd=ROOT_DIR, check=True)


class CMakeBuild(Command):
    """
    Debug (with tests): python setup.py cmake --debug
    Release (no tests): python setup.py cmake

    Generate docs: python setup.py cmake --docs
    """
    description = 'Build the C++ code with various options.'
    user_options = [
        ('debug', 'd', "Compile in debug mode."),
        ('docs', 'r', "Generate documenation for C++ and Python."),
    ]

    def initialize_options(self):
        """Initialize user options
        """
        self.debug = False
        self.docs = False

    def finalize_options(self):
        """Finalize user options
        """
        pass

    def run(self):
        """Build the C++ code.

        Raises:
            RuntimeError: Error if Cmake is not installed.
        """
        try:
            subprocess.check_output(["cmake", "--version"])
        except OSError as err:
            raise RuntimeError(
                "CMake must be installed to build musher") from err

        build_dir = get_build_dir()
        cmake_args = []

        if self.debug:
            cmake_args += ["-DCMAKE_BUILD_TYPE=Debug", "-DENABLE_TESTS=On"]
        else:
            cmake_args += ["-DCMAKE_BUILD_TYPE=Release"]

        if self.docs:
            cmake_args += ["-DGENERATE_DOCS=On"]
        else:
            cmake_args += ["-DGENERATE_DOCS=Off"]

        if not os.path.exists(build_dir):
            os.makedirs(build_dir)

        subprocess.run(['cmake', ROOT_DIR] + cmake_args,
                       cwd=build_dir,
                       check=True)
        subprocess.run(['cmake', '--build', '.'],
                       cwd=build_dir,
                       check=True)


class CTest(test):
    """Run tests if compiled (only through DEBUG)

    python setup.py cmake --debug
    python setup.py ctest
    """

    def run(self):
        build_dir = get_build_dir()

        if platform.system().lower() == "windows":
            build_dir_win_debug = os.path.join(build_dir, "Debug")
            if os.path.isdir(build_dir_win_debug):
                build_dir = build_dir_win_debug

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

        if result.returncode == -signal.SIGSEGV:
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
            bin_dir_win_debug = os.path.join(bin_dir, "Debug")
            if os.path.isdir(bin_dir_win_debug):
                bin_dir = bin_dir_win_debug

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


def extra_compile_args() -> list:
    """Platform dependent extras

    Returns:
        list: Extra compile arguments
    """
    args = []
    if platform.system() == 'Darwin':
        # Something with OS X Mojave causes libstd not to be found
        args += ['-mmacosx-version-min=10.12']

    if os.name != 'nt':
        if platform.machine() == 'i686':
            # This makes GCC generate modern SSE2 instructions
            # that give the exact IEEE 754 floating-point semantics.
            # Basically ensures that calculations using produce the
            # same results on all 32bit linux architecture.
            args += ['-msse2']
        args += ['-std=c++14']

    return args


def extra_link_args() -> list:
    """Platform dependent extras

    Returns:
        list: Extra link arguments
    """
    args = []
    if platform.system() == 'Darwin':
        # Something with OS X Mojave causes libstd not to be found
        args += ['-stdlib=libc++', '-mmacosx-version-min=10.12']

    return args


setup(
    ext_modules=[
         Extension(
             'musher.musher_python',  # Destination of .so
             include_dirs=[
                # https://caligari.dartmouth.edu/doc/ibmcxx/en_US/doc/complink/tasks/tuinclud.htm
                # Allows for root level imports within C++
                ROOT_DIR,
                # 3rd party libraries
                "src/third-party",
                pybind11.get_include()
             ],
             sources=[
                 'src/python/module.cpp',
                 'src/python/wrapper.cpp',
                 'src/python/utils.cpp',
                 'src/core/audio_decoders.cpp',
                 'src/core/utils.cpp',
                 'src/core/key.cpp',
                 'src/core/hpcp.cpp',
                 'src/core/framecutter.cpp',
                 'src/core/windowing.cpp',
                 'src/core/peak_detect.cpp',
                 'src/core/spectral_peaks.cpp',
                 'src/core/spectrum.cpp',
                 'src/core/mono_mixer.cpp'
             ],
             depends=[
                 'src/python/module.h',
                 'src/python/wrapper.h',
                 'src/python/utils.h',
                 'src/core/audio_decoders.h',
                 'src/core/utils.h',
                 'src/core/key.h'
                 'src/core/hpcp.h',
                 'src/core/framecutter.h',
                 'src/core/windowing.h',
                 'src/core/peak_detect.h',
                 'src/core/spectral_peaks.h',
                 'src/core/spectrum.h',
                 'src/core/mono_mixer.h'
             ],
             extra_compile_args=extra_compile_args(),
             extra_link_args=extra_link_args(),
         )
    ],
    cmdclass={
        "cmake": CMakeBuild,
        "ctest": CTest,
        "gtest": GTest,
        "clean": CleanProject,
        "publish_docs": PublishDocs
    },
    setup_requires=['wheel', 'cython', 'setuptools_scm>=4.1.2', 'pybind11>=2.6.0', 'numpy>=1.18.5'],
    use_scm_version=True
)
