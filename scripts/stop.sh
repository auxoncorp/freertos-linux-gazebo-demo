#!/usr/bin/env bash

set -euo pipefail

./scripts/stop_docker.sh
./scripts/killall.sh "$@"

exit 0
