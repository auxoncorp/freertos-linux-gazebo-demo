#!/bin/bash

set -euo pipefail

docker build -f docker/Dockerfile -t demo-freertos .

exit 0
