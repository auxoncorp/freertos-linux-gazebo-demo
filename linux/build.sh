#!/bin/bash

set -euo pipefail

docker build -f docker/Dockerfile -t demo-linux .

exit 0
