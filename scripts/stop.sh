#!/usr/bin/env bash

set -euo pipefail

images=("demo-gz-sim" "demo-freertos" "demo-linux")

for image in "${images[@]}"
do
    if [ -n "$(docker ps -f "name=${image}" -f "status=running" -q )" ]; then
        echo "Stopping container '${image}'"
        docker stop --time 4 $(docker ps -q --filter ancestor="${image}")
    fi
done

killall -9 modality-refelctor || true
killall -9 modality-trace-recorder-tcp-collector || true
killall -9 modality-lttng-live-collector || true

exit 0
