#!/bin/bash

make distclean
cmake -DCMAKE_BUILD_TYPE=Release .
make -j4
cpack -G DEB
