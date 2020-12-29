#!/bin/bash

THREADS=$(grep -c processor /proc/cpuinfo)

mkdir ${BUILD_DIR}
cd ${BUILD_DIR}
cmake ../ -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCODE_COVERAGE=${COVERAGE}
cmake --build . --target ${TARGET} -- -j${THREADS}