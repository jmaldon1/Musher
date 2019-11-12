#!/bin/sh

base=$PWD

root=$PWD/$1
cd $root
mkdir -p build && cd build
rm -rf linux

mkdir -p linux && cd linux

cmake -G "Unix Makefiles" $root
# make
cmake --build .

cd $base

