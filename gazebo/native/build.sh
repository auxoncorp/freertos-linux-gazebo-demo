#!/bin/bash

set -euo pipefail

mkdir -p ~/gazebo_plugins ~/gazebo_worlds

(
    cd ~/
    if [ ! -d ./modality-gazebo ]; then
        git clone --depth 1 https://github.com/auxoncorp/modality-gazebo.git --branch topic-based-interactions-prototype
    fi
    cd modality-gazebo
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
    make -j
    cp libModalityTracingPlugin.so ~/gazebo_plugins/
)

(
    cd imu_relay/
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
    make -j
    cp libImuRelay.so ~/gazebo_plugins/
)

cp -f demo_world.sdf ~/demo_world.sdf
mkdir -p ~/.gz/sim/7/
cp -f gui.config ~/.gz/sim/7/gui.config

exit 0
