#!/bin/bash

set -euo pipefail

function get_num_contacts {
    local cnt=$( modality query 'contact @ robot_chassis AGGREGATE count()' -f json | jq '.[0]["value"].Scalar' )
    echo "$cnt"
}

export MODALITY_RUN_ID="$(uuidgen)"
echo "MODALITY_RUN_ID = ${MODALITY_RUN_ID}"

RUST_LOG=error modality-reflector run --config config/reflector-config.toml --collector lttng-live --collector trace-recorder-tcp &
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

sleep 4

kill -SIGINT $refl_pid
wait $refl_pid

./scripts/stop.sh

exit 0
