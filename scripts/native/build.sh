#!/usr/bin/env bash

set -euo pipefail

(
    cd freertos
    ./native/build.sh
)

(
    cd gazebo
    ./native/build.sh
)

(
    cd linux
    ./native/build.sh
)

exit 0
