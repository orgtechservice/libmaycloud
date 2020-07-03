#!/bin/bash

make distclean
cmake -DCMAKE_BUILD_TYPE=Debug .
make -j4
cpack -G DEB
