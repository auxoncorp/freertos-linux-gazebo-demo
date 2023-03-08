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
  sudo ./scripts/setup-networking.sh
  ```
4. Export your auth token (in each terminal used)
  ```bash
  export MODALITY_AUTH_TOKEN=$(< ~/.config/modality_cli/.user_auth_token)
  ```
5. Run modality-reflector
  ```bash
  modality-reflector run --config config/reflector-config.toml --collector lttng-live --collector trace-recorder-tcp
  ```
6. Run the system (starts detached docker containers)
  ```bash
  ./scripts/start.sh
  ```
7. Press the "play" button in the Gazebo GUI to start the simulation


When your done, run the stop script.
```bash
./scripts/stop.sh
```
