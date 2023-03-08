#!/usr/bin/env bash

set -euo pipefail

# Run containers in the background
export DOCKER_OPTS="-d=true"

(
    cd freertos
    ./run.sh
)

(
    cd gazebo
    ./run.sh
)

(
    cd linux
    ./run.sh
)

exit 0
