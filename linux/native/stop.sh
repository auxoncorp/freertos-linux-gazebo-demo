#!/bin/bash

set -euo pipefail

pkill -f 'actuator-app'
sleep 2
pkill -9 -f 'actuator-app'

exit 0
