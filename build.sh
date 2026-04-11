#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/engine/build"
CONFIG="Debug"

for arg in "$@"; do
    [[ "$arg" == "--release" ]] && CONFIG="Release"
done

cmake --build "$BUILD_DIR" --config $CONFIG -- -j"$(nproc 2>/dev/null || sysctl -n hw.logicalcpu)"