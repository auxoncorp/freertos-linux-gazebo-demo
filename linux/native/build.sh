#!/bin/bash

set -euo pipefail

(
    cd actuator/
    mkdir -p build
    cd build/
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
    make -j
    sudo make install
)

(
    mkdir -p build
    cd build/
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
    make -j
    sudo make install
)

exit 0
