#!/usr/bin/env bash

set -euo pipefail

# The run-until-contact.sh will give the reflector PID for graceful shutdown
if [ $# -ge 1 ] && [ -n "$1" ]; then
    kill -SIGINT $1
    while [ -e /proc/$1 ]; do sleep 1; done
fi

killall -9 modality-reflector || true
killall -9 modality-trace-recorder-tcp-collector || true
killall -9 modality-lttng-live-collector || true

exit 0
