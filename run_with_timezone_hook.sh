#!/bin/bash
# Run Antidetect Browser with timezone hook

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
HOOK_LIB="$SCRIPT_DIR/src/timezone_hook/libtimezone_hook.so"
BROWSER="$SCRIPT_DIR/build/AntidetectBrowser"

if [ ! -f "$HOOK_LIB" ]; then
    echo "❌ Timezone hook library not found: $HOOK_LIB"
    echo "Building it now..."
    cd "$SCRIPT_DIR/src/timezone_hook" && ./build.sh
    if [ $? -ne 0 ]; then
        echo "❌ Failed to build timezone hook"
        exit 1
    fi
fi

if [ ! -f "$BROWSER" ]; then
    echo "❌ Browser executable not found: $BROWSER"
    exit 1
fi

echo "🚀 Starting Antidetect Browser with timezone hook..."
echo "📍 Hook library: $HOOK_LIB"
echo "📍 Browser: $BROWSER"

# Run with LD_PRELOAD
LD_PRELOAD="$HOOK_LIB" "$BROWSER" "$@"
