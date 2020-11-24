# Build wheels for every linux distro, mac, and windows

## Tool

https://github.com/joerick/cibuildwheel

## Demo

https://github.com/joerick/cibuildwheel-autopypi-example


## Local creation of wheels (Linux wheels only)

### Dependencies

1. [Docker](https://www.docker.com/)
2. `pip install cibuildwheel`


### Set variables

Edit the `tools/.cibuildwheel.env` file with the desired settings. Then export with the following command:

```shell
$ source tools/.cibuildwheel.env
```

### Run

```shell
$ cibuildwheel --platform linux
```

### Notes

Wheels output will be stored in the project root `wheelhouse/*.whl`


## CI/CD Github Actions creation of wheels

### Config file

Ensure config file exists in `.github/workflows/`

### Conditions

1. Building and testing occurs on all pushes and pull requests to the repo.
2. Uploading to PyPi only occurs on tagged commits that begin with a `v`. eg. `v1.0.0`
