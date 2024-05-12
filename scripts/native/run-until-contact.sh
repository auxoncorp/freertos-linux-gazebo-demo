#!/bin/bash

set -euo pipefail

function get_num_contacts {
    local cnt=$( modality query 'contact @ robot_chassis AGGREGATE count()' -f json | jq '.[0]["value"].Scalar' )
    echo "$cnt"
}

run_id=$(./scripts/get_test_run_id.sh)
export MODALITY_RUN_ID="${MODALITY_RUN_ID:-${run_id}}"
echo "MODALITY_RUN_ID = ${MODALITY_RUN_ID}"

RUST_LOG=error modality-reflector run --config config/native/reflector-config.toml --collector lttng-live --collector trace-recorder-tcp &
refl_pid=$!

sleep 2

./scripts/native/start.sh

sleep 10

while [ $(get_num_contacts) -eq 0 ]
do
    echo "Waiting for chassis contact"
    sleep 2
done

echo "Contact!"

sleep 10

kill -SIGINT $refl_pid
wait $refl_pid

# TODO
#./scripts/stop.sh

modality internal sync-indices

exit 0
