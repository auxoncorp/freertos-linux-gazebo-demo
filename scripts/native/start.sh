#!/usr/bin/env bash

set -euo pipefail

(
    cd freertos
    ./native/run.sh &
)

(
    cd gazebo
    ./native/run.sh &
)

(
    cd linux
    ./native/run.sh &
)

exit 0
