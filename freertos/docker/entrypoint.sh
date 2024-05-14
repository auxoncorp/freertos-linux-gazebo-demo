#!/bin/bash

set -euo pipefail

trap ctrl_c INT TERM

stopped=0
ctrl_c() {
    echo "Shutting down"
    stopped=1
}

RENODE_OPTS="${RENODE_OPTS:-}"
RENODE_TELNET_PORT="${RENODE_TELNET_PORT:-54543}"

if [ -v DEMO_HEADLESS ]; then
    echo "Renode monitor is available on port ${RENODE_TELNET_PORT}"
    RENODE_OPTS="${RENODE_OPTS} --disable-xwt --port=${RENODE_TELNET_PORT}"
fi

nonce="${FREERTOS_STARTUP_NONCE:-0}"
enable_voltage_spike="${VOLTAGE_SPIKE_ENABLED:-1}"

renode ${RENODE_OPTS} -e "\$startup_nonce=$nonce ; \$voltage_spike_enabled=$enable_voltage_spike ; include @/app/emulate.resc" &
pid=$!
while [ $stopped -eq 0 ]; do
    sleep 1
done

kill $pid
wait $pid

exit 0
