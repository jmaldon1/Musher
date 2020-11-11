# Musher

Simple musical key detection.

# Table of Contents

<!-- MarkdownTOC -->

- [Musher](#musher)
- [Table of Contents](#table-of-contents)
- [Installation](#installation)
  - [Python (WIP)](#python-wip)
  - [C++](#c)
    - [Required General Dependencies:](#required-general-dependencies)
      - [CMAKE](#cmake)
      - [Conan](#conan)
    - [Install (WIP)](#install-wip)
- [Usage Section (WIP)](#usage-section-wip)
- [Development](#development)
  - [Python](#python)
    - [Install normally](#install-normally)
    - [Install in debug mode](#install-in-debug-mode)
  - [C++](#c-1)
    - [Install normally](#install-normally-1)
    - [Install in debug mode](#install-in-debug-mode-1)
- [Tests](#tests)
  - [Python](#python-1)
    - [Required dependencies](#required-dependencies)
    - [Running tests](#running-tests)
  - [C++](#c-2)
- [Cleanup](#cleanup)
- [Useful links](#useful-links)

<!-- /MarkdownTOC -->


# Installation

You can choose to install this package in 2 ways, Python and C++. The package was primary written in C++ and wrapped in Python. Choose either of the following methods of installation.


## Python (WIP)

```
pip install musher
```

## C++

### Required General Dependencies:

#### CMAKE

```
-- MacOS --
brew install cmake

-- Linux --
sudo apt install cmake

-- Windows --
Download from https://cmake.org/download/
```

#### Conan

```
pip install conan
```

### Install (WIP)

```
conan install musher
```


# Usage Section (WIP)

# Development

## Python

### Install normally

```sh
pip install .
# OR
python3 setup.py install
```

### Install in debug mode

```sh
pip install -e .
# OR
python3 setup.py develop
```

_NOTE_: This package has 2 dependencies: **Pybind11** and **Numpy**. You may need to install these python packages if the setup.py does not do it for you.

## C++

### Install normally

```sh
python setup.py cmake

# OR

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

```

### Install in debug mode

```sh
python setup.py cmake --debug

# OR

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=On
cmake --build .
```

# Tests

## Python

### Required dependencies

```sh
pip install tox pytest
```

### Running tests
This will run tests against a pip installed copy of the source code (best to do before deploying code).

```sh
# Running this command does not require pytest to be installed
tox 
```

This will run tests directly (best to do while modifying the code base).

```sh
pytest ./tests -v
```

## C++

```sh
# Ctest
python setup.py ctest

# OR

# Google test
python setup.py gtest
```

# Cleanup

This will remove all the temporary files/folders created from building and testing this package.

```sh
python setup.py clean
```

# Useful links

Python audio libraries:

* https://github.com/vinta/awesome-python#audio