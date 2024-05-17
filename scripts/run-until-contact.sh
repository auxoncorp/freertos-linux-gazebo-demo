#!/bin/bash

set -euo pipefail

function get_num_contacts {
    local cnt=$( modality query 'contact@robot_chassis -> publish_message@robot_chassis AGGREGATE count()' -f json | jq '.[0]["value"].Scalar' )
    echo "$cnt"
}

run_id=$(./scripts/get_test_run_id.sh)
export MODALITY_RUN_ID="${MODALITY_RUN_ID:-${run_id}}"
echo "MODALITY_RUN_ID = ${MODALITY_RUN_ID}"

RUST_LOG=error modality-reflector run --config config/reflector-config.toml --collector lttng-live --collector trace-recorder-tcp &
refl_pid=$!

curl --retry-max-time 30 --retry 10 --retry-connrefused http://localhost:14188 2>/dev/null 1>&2 || true

./scripts/start.sh

sleep 2

while [ $(get_num_contacts) -eq 0 ]
do
    echo "Waiting for chassis contact"
    sleep 2
done

echo "Contact!"

./scripts/stop.sh $refl_pid

modality internal sync-indices

exit 0
