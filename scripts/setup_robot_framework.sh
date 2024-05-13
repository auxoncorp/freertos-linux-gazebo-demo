#!/usr/bin/env bash

set -euo pipefail

if [ ! -d modality-robot-framework ]; then
    git clone --depth 1 https://github.com/auxoncorp/modality-robot-framework.git
fi

python3 -m venv .env
source .env/bin/activate

pip install maturin
pip install patchelf
pip install robotframework==6.0.2
pip install robotframework-retryfailed==0.2.0
pip install psutil==5.9.3
pip install pyyaml==6.0

(
    cd modality-robot-framework
    maturin develop --release
)

exit 0
