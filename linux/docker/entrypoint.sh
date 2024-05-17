#!/bin/bash

set -euo pipefail

trap ctrl_c INT TERM

stopped=0
ctrl_c() {
    echo "Shutting down"
    stopped=1
}

lttng-relayd \
    --background \
    -o ${LTTNG_HOME}/lttng/lttng-traces \
    --live-port=tcp://0.0.0.0:5344 \
    --control-port=tcp://0.0.0.0:5342 \
    --data-port=tcp://0.0.0.0:5343

lttng create linux --live=100000 --shm-path /lttng/shm
lttng enable-event --userspace actuator:'*'
lttng start

actuator-app &
pid=$!
while [ $stopped -eq 0 ]; do
    sleep 1
done

kill -SIGTERM $pid
wait $pid
sleep 1

lttng stop
lttng destroy

exit 0
