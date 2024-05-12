#!/bin/bash

set -euo pipefail

if [ -v DEMO_HEADLESS ]; then
    gz sim -r --headless-rendering -s demo_world.sdf
else
    gz sim -r demo_world.sdf
fi

exit 0
