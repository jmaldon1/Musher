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