#!/bin/bash

set -e

THREADS=$(grep -c processor /proc/cpuinfo)

echo "Building reactor...";
echo "Build dir: ${BUILD_DIR}"
echo "Build type: ${BUILD_TYPE}"
echo "C++11: ${CXX11}"
echo "Coverage: ${COVERAGE}"
echo "Make jobs: ${THREADS}"

mkdir ${BUILD_DIR}
cd ${BUILD_DIR}
cmake ../ -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCXX11_ENABLED=${CXX11} -DCODE_COVERAGE=${COVERAGE}
cmake --build . --target ${TARGET} -- -j${THREADS}
if [ "ON" == "${COVERAGE}" ]; then
   cmake --build . --target ccov -- -j${THREADS}
   cmake --build . --target ccov-report -- -j${THREADS}
fi