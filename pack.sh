#!/bin/bash

set -e

mkdir build-pack
cd build-pack
cmake ../ -DGENERATE_VERSION=ON
cpack --config CPackSourceConfig.cmake
