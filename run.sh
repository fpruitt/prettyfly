#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/engine/build"
EXE="$BUILD_DIR/PrettyFly"
RELEASE=0

for arg in "$@"; do
    [[ "$arg" == "--release" ]] && RELEASE=1
done

if [[ ! -f "$EXE" ]]; then
    echo -e "\033[0;36m[run] Binary not found, building first...\033[0m"
    "$SCRIPT_DIR/build.sh" $([ $RELEASE -eq 1 ] && echo "--release")
fi

"$EXE"