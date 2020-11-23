#!/bin/bash
set -e -u -x

PACKAGE=musher
PROJECT_DIR=/io/
WHEELHOUSE=$PROJECT_DIR/wheelhouse

is_unsupported_python() {
    # Only Python 3.5+ is supported
    if python -c 'import sys; sys.exit(not sys.version_info >= (3, 5))'; then
        return 0
    fi
    return 1
}

prepare_system() {
    echo "Python versions found: $(cd /opt/python && echo cp* | sed -e 's|[^ ]*-||g')"
    ${CC:-gcc} --version
}

build_wheels() {
    for PYBIN in /opt/python/*/bin; do
        if is_unsupported_python; then
            continue
        fi
        "${PYBIN}/pip" wheel $PROJECT_DIR --no-deps -w $WHEELHOUSE
    done
}

run_tests() {
    # Install packages and test
    for PYBIN in /opt/python/*/bin/; do
        if is_unsupported_python; then
            continue
        fi
        "${PYBIN}/pip" install -r $PROJECT_DIR/dev_requirements.txt
        ${PYBIN}/python -m pip install $PACKAGE --no-index -f $WHEELHOUSE || exit 1

        (cd $PROJECT_DIR; "${PYBIN}/pytest" tests -v)
    done
}

show_wheels() {
    ls -l $WHEELHOUSE/*.whl
}

prepare_system
build_wheels
run_tests
show_wheels
