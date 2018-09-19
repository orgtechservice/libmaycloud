#!/bin/bash

make distclean
cmake -DCMAKE_BUILD_TYPE=Debug .
make deb
