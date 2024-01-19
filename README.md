# System Demo

## Overview

![overview.png](diagrams/overview.png)

## Getting Started

1. Setup the workspace
  ```bash
  modality workspace create --use demo config/workspace.toml
  ```
2. Build the docker images and applications
  ```bash
  ./scripts/build.sh
  ```
3. In a separate terminal, run the networking setup script
  ```bash
  sudo ./scripts/setup-network.sh
  ```
4. Export your auth token (in each terminal used)
  ```bash
  export MODALITY_AUTH_TOKEN=$(< ~/.config/modality_cli/.user_auth_token)
  ```
5. Run the system (starts modality-reflector and the detached docker containers), automatically stops after contact is made in gazebo
  ```bash
  # Set DEMO_HEADLESS env var to not run any of the GUIs (gazebo, renode, etc)
  # Set DOCKER_WITHOUT_NVIDIA env var to omit GPU support in docker
  ./scripts/run-until-contact.sh
  ```
