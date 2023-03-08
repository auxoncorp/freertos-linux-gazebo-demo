#!/bin/bash

set -euo pipefail

docker build -f docker/Dockerfile -t demo-gz-sim .

exit 0
