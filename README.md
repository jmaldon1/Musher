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
        - [How to run tests](#how-to-run-tests)
    - [Accomplishments](#accomplishments)
    - [Next Steps](#next-steps)
    - [Useful links](#useful-links)
    - [~~Build the Proof of Concept Model~~ \(OLD INSTRUCTIONS\)](#%7E%7Ebuild-the-proof-of-concept-model%7E%7E-old-instructions)
        - [Windows](#windows)
            - [Required General Dependencies](#required-general-dependencies-1)
            - [Build](#build)
        - [Linux & MacOS](#linux--macos)
            - [Required General Dependencies](#required-general-dependencies-2)
            - [Build](#build-1)

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
```shell
brew install cmake
```

**Linux**
```shell
sudo apt install cmake
```

**Windows**

Download from https://cmake.org/download/


### Build & install python module

#### Install normally

```shell
pip install .
# OR
python3 setup.py install
```

#### Install in development mode

```shell
pip install -e .
# OR
python3 setup.py develop
```

## Tests

### Required modules

```shell
pip install pytest
```

### How to run tests

```shell
pytest ./tests
# OR
python3 setup.py test
```


## Accomplishments

1. Get the python packages to compile using precompiled static and/or shared libraries instead of recompiling the c++ source code on linux and mac.

2. Added support for mac and linux having multiple python versions, this used to cause a seg fault. Along with adding support for windows, which used to not work at all.

## Next Steps

1. Start the actual project (This will be broken down later)

## Useful links

Python audio libraries:

* https://github.com/vinta/awesome-python#audio




## ~~Build the Proof of Concept Model~~ (OLD INSTRUCTIONS)

### Windows

#### Required General Dependencies

blah blah

#### Build
1. Run 

    ```
    build_win.bat poc\c++_extensions
    ```

    to generate and build the Visual Studio 2019 (vc16/ toolset 142) projects.

2. Within the directory `poc\c++_extensions\song\wrapper`, run

    ```shell
    python setup.py build_ext --inplace
    ```

3. Run the python test program

    ```shell
    python song_test.py
    ```

### Linux & MacOS

#### Required General Dependencies

**MacOS**
```shell
brew install cmake
```

**Linux**
```shell
sudo apt install cmake
```

#### Build

1. Run 

    ```shell
    bash build_linux.sh poc/c++_extensions
    ```
    to generate and build the Unix Makefile based projects.

2. Within the directory `poc/c++_extensions/song/wrapper`, run

    ```shell
    python3 setup.py build_ext --inplace
    ```

3. Run the python test program

    ```shell
    python3 song_test.py
    ```
