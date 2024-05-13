#!/usr/bin/env bash

set -euo pipefail

modality user create --use admin

modality workspace create --use demo config/workspace.toml

modality segment use --latest

conform spec create --file specs/system.speqtr system
conform spec create --file specs/rtos_resources.speqtr rtos_resources

exit 0
