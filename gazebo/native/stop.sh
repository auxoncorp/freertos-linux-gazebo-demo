#!/bin/bash

set -euo pipefail

pkill -f 'gz sim'
sleep 2
pkill -9 -f 'gz sim'

exit 0
