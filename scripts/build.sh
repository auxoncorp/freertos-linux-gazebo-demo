#!/usr/bin/env bash

set -euo pipefail

(
    cd freertos
    ./build.sh
)

(
    cd gazebo
    ./build.sh
)

(
    cd linux
    ./build.sh
)

exit 0
