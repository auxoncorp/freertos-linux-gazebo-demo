#!/bin/bash

set -euo pipefail

(
    mkdir -p build
    cd build
    cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/gcc-toolchain.cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
    make -j
)

exit 0
