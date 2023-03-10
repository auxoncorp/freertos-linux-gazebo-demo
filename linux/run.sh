#!/bin/bash

set -euo pipefail

DOCKER_OPTS="${DOCKER_OPTS:-"-it"}"

container_image=${1:-demo-linux}
container_id=$(docker ps -aqf "ancestor=${container_image}")

if [ -z "$container_id" ]; then
    container_name="demo-linux"
    docker run \
        --rm \
        --network=host \
        --env GZ_PARTITION=demo \
        --security-opt seccomp=unconfined \
        --hostname "demo-linux" \
        $DOCKER_OPTS \
        --name "$container_name" \
        $container_image
else
    docker exec --privileged -it $container_id bash
fi

exit 0
