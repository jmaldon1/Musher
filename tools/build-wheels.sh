#!/bin/bash
set -e -u -x

PACKAGE=musher
PROJECT_DIR=/io/
WHEELHOUSE=$PROJECT_DIR/wheelhouse

prepare_system() {
    echo "Python versions found: $(cd /opt/python && echo cp* | sed -e 's|[^ ]*-||g')"
    ${CC:-gcc} --version
}

build_wheels() {
    for PYBIN in /opt/python/*/bin; do
        "${PYBIN}/pip" wheel $PROJECT_DIR --no-deps -w $WHEELHOUSE
    done
}

run_tests() {
    # Install packages and test
    for PYBIN in /opt/python/*/bin/; do
        "${PYBIN}/pip" install -r $PROJECT_DIR/dev_requirements.txt
        ${PYBIN}/python -m pip install $PACKAGE --no-index -f $WHEELHOUSE || exit 1

        (cd $PROJECT_DIR; "${PYBIN}/pytest" tests -v)
    done
}

prepare_system
build_wheels
run_tests
