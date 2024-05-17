#!/usr/bin/env bash

set -euo pipefail

remote="192.0.2.80"
ping_cancelled=false
until ping -c1 "$remote" >/dev/null 2>&1; do :; done &    # The "&" backgrounds it
trap "kill $!; ping_cancelled=true" SIGINT
wait $!
trap - SIGINT

exit 0
