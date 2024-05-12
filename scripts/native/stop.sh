#!/usr/bin/env bash

(
    cd freertos
    ./native/stop.sh
)

(
    cd gazebo
    ./native/stop.sh
)

(
    cd linux
    ./native/stop.sh
)

killall -9 modality-refelctor modality-trace-recorder-tcp-collector modality-lttng-live-collector

exit 0
