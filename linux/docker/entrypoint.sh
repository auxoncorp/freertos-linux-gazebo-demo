#!/bin/bash

set -euo pipefail

lttng-relayd \
    --background \
    -o ${LTTNG_HOME}/lttng/lttng-traces \
    --live-port=tcp://0.0.0.0:5344 \
    --control-port=tcp://0.0.0.0:5342 \
    --data-port=tcp://0.0.0.0:5343

lttng create linux --live --shm-path /lttng/shm
lttng enable-event --userspace actuator:'*'
lttng start

actuator-app

sleep 1

lttng stop
lttng destroy

exit 0
