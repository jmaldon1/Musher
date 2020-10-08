<!-- MarkdownTOC -->

- [Musher](#musher)
  - [Steps](#steps)
  - [Installation](#installation)
    - [Required General Dependencies](#required-general-dependencies)
    - [Build & install python module](#build--install-python-module)
      - [Install normally](#install-normally)
      - [Install in development mode](#install-in-development-mode)
  - [Tests](#tests)
    - [Required modules](#required-modules)
    - [How to run Python tests](#how-to-run-python-tests)
    - [How to run C++ tests](#how-to-run-c-tests)
  - [Cleanup](#cleanup)
  - [Accomplishments](#accomplishments)
  - [Next Steps](#next-steps)
  - [Useful links](#useful-links)

<!-- /MarkdownTOC -->


# Musher
Mush songs together to create new songs.

## Steps

1. Get all metadata out of various song files.

2. Combine like pieces of songs together thru their metadata

3. Put songs back together. 


## Installation

### Required General Dependencies

**MacOS**
```sh
brew install cmake
```

**Linux**
```sh
sudo apt install cmake
```

**Windows**

Download from https://cmake.org/download/


### Build & install python module

#### Install normally

```sh
pip install .
# OR
python3 setup.py install
```

#### Install in development mode

```sh
pip install -e .
# OR
python3 setup.py develop
```

## Tests

### Required modules

```sh
pip install tox pytest
```

### How to run Python tests

This will run tests against a pip installed copy of the source code (best to do before deploying code)

```sh
# Running this command does not require pytest to be installed
tox 
```

This will run tests directly (best to do while modifying the code base)

```sh
pytest ./tests
```

Watch test file changes and rerun pytest on save (Requires [entr](https://bitbucket.org/eradman/entr/src/default/))

[Entr installation instructions](https://bitbucket.org/eradman/entr/src/default/)

```sh
find ./tests \( -iname \*.py -o -iname \*.conf \) | entr pytest

find . \( -iname \*.h -o -iname \*.cpp \) | entr python setup.py build_cpp_tests --r
```

### How to run C++ tests

This will create an executables of the C++ tests and save them to the `test_bin` folder.

```sh
# This will simply build the tests
python setup.py build_cpp_tests

# This will run the tests
./test_musher_cpp

# This will build & run the tests (--run-tests or --r)
python setup.py build_cpp_tests --r
```

The alternative Cmake way:

```sh
# Build Tests
mkdir build
cd build
cmake .. -DBUILD_PYTHON_MODULE=OFF
cmake --build .

# Run Test
../test_bin/test_musher_library
# OR
ctest
# OR
make test
```

_NOTE: If your CMake version is less than 3.11 than you must clone the googletest submodules in order for tests to work._

```sh
git submodule update --init --recursive
```

## Cleanup

This will remove all the files created from building and testing this package

```sh
python setup.py clean
```

## Accomplishments

1. Get the python packages to compile using precompiled static and/or shared libraries instead of recompiling the c++ source code on linux and mac.

2. Added support for mac and linux having multiple python versions, this used to cause a seg fault. Along with adding support for windows, which used to not work at all.

3. Seperate the C++ tests from the python module so that we can ship the python code without the c++ tests

4. Solved a linking issue with MacOSX where musher_library (shared library .dylib) would not link with the python module (shared library .so). We changed musher_library to STATIC instead of SHARED and the linking issue went away.

## Next Steps

1. Make sure everything works with windows and linux before moving on.

2. Figure out how to pass python function to C++ function

## Useful links

Python audio libraries:

* https://github.com/vinta/awesome-python#audio