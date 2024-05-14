#!/bin/bash

set -euo pipefail

if [ -v DEMO_HEADLESS ]; then
    exec gz sim -r --headless-rendering -s demo_world.sdf
else
    exec gz sim -r demo_world.sdf
fi

exit 0
