#!/bin/bash

make distclean
cmake3 -DCMAKE_BUILD_TYPE=Release -DDISABLE_HTTP=1 .
make -j4
cpack3 -G RPM
