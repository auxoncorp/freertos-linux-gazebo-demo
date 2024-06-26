#!/bin/bash

set -euo pipefail

if [ ! -v MODALITY_AUTH_TOKEN ]; then
    export MODALITY_AUTH_TOKEN=$(modality user auth-token)
fi

xauth=/tmp/.docker.xauth
if [ ! -v DOCKER_WITHOUT_NVIDIA ] && [ ! -f "$xauth" ]; then
    xauth_list=$(sed -e 's/^..../ffff/' <<< "$(xauth nlist $DISPLAY)")
    if [ ! -z "$xauth_list" ]; then
        echo "$xauth_list" | xauth -f "$xauth" nmerge -
    else
        touch "$xauth"
    fi
    chmod a+r "$xauth"
fi

DOCKER_OPTS="${DOCKER_OPTS:-"-it"}"
if [ ! -v DOCKER_WITHOUT_NVIDIA ]; then
    docker_version=$(docker version --format '{{.Server.Version}}')
    if dpkg --compare-versions 19.03 gt "$docker_version" ; then
        echo "Docker version is less than 19.03, using nvidia-docker2 runtime"
        if [ ! dpkg --list | grep nvidia-docker2 ]; then
            echo "Update docker to a version greater than 19.03 or install nvidia-docker2"
            exit 1
        fi
        DOCKER_OPTS="$DOCKER_OPTS --runtime=nvidia"
    else
        DOCKER_OPTS="$DOCKER_OPTS --gpus all"
    fi
else
    DOCKER_OPTS="$DOCKER_OPTS --device /dev/dri"
fi

container_image=${1:-demo-gz-sim}
container_id=$(docker ps -aqf "ancestor=${container_image}")

if [ -z "$container_id" ]; then
    container_name="demo-gz-sim"
    docker run \
        --rm \
        --network=host \
        --name "$container_name" \
        $DOCKER_OPTS \
        --env GZ_PARTITION=demo \
        --env DISPLAY \
        --env XAUTHORITY="$xauth" \
        --env MODALITY_AUTH_TOKEN \
        --env MODALITY_RUN_ID \
        --env DEMO_HEADLESS \
        --volume=/tmp/.X11-unix:/tmp/.X11-unix:ro \
        --volume="$xauth:$xauth" \
        $container_image
else
    docker exec --privileged -e DISPLAY -it $container_id bash
fi

exit 0
