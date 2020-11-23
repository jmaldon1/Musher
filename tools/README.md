# Build wheels for every linux distro

## Demo

https://github.com/pypa/python-manylinux-demo

## Local creation of wheels

### Dependency

[Docker](https://www.docker.com/)

### Set variables

```shell
$ DOCKER_IMAGE='quay.io/pypa/manylinux2014_x86_64'
$ PLAT='manylinux2014_x86_64'
```

```shell
$ docker container run -t --rm -e PLAT=$PLAT -v "$(pwd)":/io "$DOCKER_IMAGE" /io/tools/build-wheels.sh
```

### Notes

`/io/` is the volume mount on the docker image for this project.

The docker container will run the `tools/build-wheels.sh` script.

Output will be stored in the project root `./wheelhouse/*.whl`


## CI/CD Travis creation of wheels