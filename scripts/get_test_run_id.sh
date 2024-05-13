#!/usr/bin/env bash

set -euo pipefail

dir=".runids"
test_name="default"
runid_file="$dir/$test_name"

mkdir -p "$dir"

runid=0

if [ -f "$runid_file" ]; then
    runid=$(cat "$runid_file")
fi

runid=$((runid+1))

echo "$runid" > "$runid_file"

echo "$runid"

exit 0
