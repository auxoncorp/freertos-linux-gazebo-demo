#!/bin/bash

set -euo pipefail

RENODE_OPTS="${RENODE_OPTS:-}"
RENODE_TELNET_PORT="${RENODE_TELNET_PORT:-54543}"

if [ -v DEMO_HEADLESS ]; then
    echo "Renode monitor is available on port ${RENODE_TELNET_PORT}"
    RENODE_OPTS="${RENODE_OPTS} --disable-xwt --port=${RENODE_TELNET_PORT}"
fi

nonce="${FREERTOS_STARTUP_NONCE:-0}"

renode ${RENODE_OPTS} -e "\$startup_nonce=$nonce ; include @/app/emulate.resc"

exit 0
