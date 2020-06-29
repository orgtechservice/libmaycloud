#!/bin/bash

make distclean
cmake3 -DCMAKE_BUILD_TYPE=Release .
make -j4
cpack3
