#!/bin/bash

set -euo pipefail

DOCKER_OPTS="${DOCKER_OPTS:-}"

container_image=${1:-demo-linux}
container_id=$(docker ps -aqf "ancestor=${container_image}")

if [ -z "$container_id" ]; then
    container_name="demo-linux"
    docker run \
        -it --rm \
        --network=host \
        -e GZ_PARTITION=demo \
        --security-opt seccomp=unconfined \
        -h "demo-linux" \
        $DOCKER_OPTS \
        --name "$container_name" \
        $container_image
else
    docker exec --privileged -e DISPLAY -it $container_id bash
fi

exit 0
