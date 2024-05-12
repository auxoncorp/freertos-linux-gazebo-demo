#!/bin/bash

set -euo pipefail

pkill -f 'renode'
sleep 2
pkill -9 -f 'renode'

exit 0
