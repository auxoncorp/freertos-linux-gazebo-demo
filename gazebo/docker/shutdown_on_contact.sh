#!/bin/bash

set -euo pipefail

# Wait for gz sim to start
sleep 2

# Wait for the 800 contact events (note that we're sampling every 100 iterations)
gz topic -e -t /world/Moving_robot/model/vehicle_blue/link/chassis/sensor/chassis_contact/contact --num 800

# The entrypoint.sh script exec's gz sim, it'll always be PID 1 inside the docker container
kill 1

exit 0
