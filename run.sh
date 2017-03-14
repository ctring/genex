#!/bin/bash

# set the compiler to clang instead of g++ (https://cmake.org/pipermail/cmake/2012-September/052085.html)
export CXX=/usr/bin/clang++
mkdir -p build
cd build
cmake ../src/
make
make test
cd ..

