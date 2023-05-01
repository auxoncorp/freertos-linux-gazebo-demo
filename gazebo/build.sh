#!/bin/bash

set -euo pipefail

(
    cd imu_relay
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
    make -j
)

exit 0
