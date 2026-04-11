#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VCPKG_PATH="${1:-$SCRIPT_DIR/tools/vcpkg}"
BUILD_DIR="$SCRIPT_DIR/engine/build"

log()     { echo -e "\033[0;36m[setup] $1\033[0m"; }
success() { echo -e "\033[0;32m[setup] $1\033[0m"; }
fail()    { echo -e "\033[0;31m[setup] $1\033[0m"; exit 1; }

# ── system dependencies ───────────────────────────────────────────────────────

install_system_deps() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        if ! command -v brew &>/dev/null; then
            fail "Homebrew not found. Install it from https://brew.sh first."
        fi
        log "Installing system dependencies via Homebrew..."
        brew install cmake git pkg-config

    elif command -v apt-get &>/dev/null; then
        log "Installing system dependencies via apt..."
        sudo apt-get update -q
        sudo apt-get install -y cmake git curl zip unzip tar pkg-config \
            libgl1-mesa-dev libxrandr-dev libxinerama-dev \
            libxcursor-dev libxi-dev libxxf86vm-dev

    elif command -v pacman &>/dev/null; then
        log "Installing system dependencies via pacman..."
        sudo pacman -Sy --needed cmake git base-devel mesa libx11 \
            libxrandr libxinerama libxcursor libxi

    else
        fail "Unsupported package manager. Install cmake, git, and OpenGL headers manually."
    fi
}

install_system_deps

# ── vcpkg ─────────────────────────────────────────────────────────────────────

if [[ -f "$VCPKG_PATH/vcpkg" ]]; then
    log "vcpkg already bootstrapped, skipping."
else
    if [[ ! -d "$VCPKG_PATH" ]]; then
        log "Cloning vcpkg..."
        git clone https://github.com/microsoft/vcpkg.git "$VCPKG_PATH"
    fi
    log "Bootstrapping vcpkg..."
    "$VCPKG_PATH/bootstrap-vcpkg.sh" -disableMetrics
fi


# ── CMake configure ───────────────────────────────────────────────────────────

TOOLCHAIN="$VCPKG_PATH/scripts/buildsystems/vcpkg.cmake"

mkdir -p "$BUILD_DIR"

log "Running CMake configure..."
cmake -S "$SCRIPT_DIR/engine" -B "$BUILD_DIR" \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
    -DCMAKE_BUILD_TYPE=Debug

log "Building..."
cmake --build "$BUILD_DIR" -- -j"$(nproc 2>/dev/null || sysctl -n hw.logicalcpu)"

success "Setup complete! Run: ./$BUILD_DIR/PrettyFly"