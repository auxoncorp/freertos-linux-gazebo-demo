#!/usr/bin/env bash

set -euo pipefail

if [ ! -d .env ]; then
    echo "python venv is missing, did you run scripts/setup_robot_framework.sh?"
    exit 1
fi

source .env/bin/activate
robot --consolewidth 90 --console verbose test_suite.robot

exit 0
