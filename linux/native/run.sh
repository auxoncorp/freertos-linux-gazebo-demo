#!/bin/bash

set -euo pipefail

mkdir -p "$LTTNG_HOME"

lttng create linux --live
lttng enable-event --userspace actuator:'*'
lttng start

# Runs until SIGINT is recvd
actuator-app

sleep 1

lttng stop
lttng destroy

exit 0
