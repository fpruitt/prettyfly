#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/engine/build"
TOOLCHAIN="$SCRIPT_DIR/tools/vcpkg/scripts/buildsystems/vcpkg.cmake"
CONFIG="Debug"

for arg in "$@"; do
    [[ "$arg" == "--release" ]] && CONFIG="Release"
done

if [[ -d "$BUILD_DIR" ]]; then
    rm -rf "$BUILD_DIR"
fi
mkdir -p "$BUILD_DIR"

cmake -S "$SCRIPT_DIR/engine" -B "$BUILD_DIR" \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
    -DCMAKE_BUILD_TYPE=$CONFIG