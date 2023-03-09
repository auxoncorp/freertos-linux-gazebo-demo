#!/bin/bash

set -euo pipefail

function get_num_contacts {
    local cnt=$( modality query 'contact @ robot_chassis AGGREGATE count()' -f json | jq '.[0]["value"].Scalar' )
    echo "$cnt"
}

export MODALITY_RUN_ID="$(uuidgen)"
echo "MODALITY_RUN_ID = ${MODALITY_RUN_ID}"

# TODO - use the reflector docker image
# https://github.com/auxoncorp/freertos-linux-gazebo-demo/issues/1
modality-reflector run --config config/reflector-config.toml --collector lttng-live --collector trace-recorder-tcp &
refl_pid=$!

sleep 2

./scripts/start.sh

sleep 1

while [ $(get_num_contacts) -eq 0 ]
do
    echo "Waiting for chassis contact"
    sleep 2
done

echo "Contact!"

sleep 2

./scripts/stop.sh

kill -SIGINT $refl_pid
wait $refl_pid

exit 0
