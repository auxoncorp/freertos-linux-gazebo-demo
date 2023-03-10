#!/bin/bash

set -euo pipefail

RENODE_OPTS="${RENODE_OPTS:-}"
RENODE_TELNET_PORT="${RENODE_TELNET_PORT:-54543}"

if [ -v DEMO_HEADLESS ]; then
    echo "Renode monitor is available on port ${RENODE_TELNET_PORT}"
    RENODE_OPTS="${RENODE_OPTS} --disable-xwt --port=${RENODE_TELNET_PORT}"
fi

cmd="renode ${RENODE_OPTS} /app/emulate.resc"
$cmd

exit 0
