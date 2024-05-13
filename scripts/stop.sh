#!/usr/bin/env bash

set -euo pipefail

images=("demo-gz-sim" "demo-freertos" "demo-linux")

for image in "${images[@]}"
do
    if [ -n "$(docker ps -f "name=${image}" -f "status=running" -q )" ]; then
        echo "Stopping container '${image}'"
        docker stop --time 3 $(docker ps -q --filter ancestor="${image}")
    fi
done

exit 0
