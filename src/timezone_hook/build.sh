#!/bin/bash
# Build timezone hook library

gcc -shared -fPIC -o libtimezone_hook.so timezone_hook.c -ldl

if [ $? -eq 0 ]; then
    echo "✅ libtimezone_hook.so built successfully"
    echo "📍 Location: $(pwd)/libtimezone_hook.so"
else
    echo "❌ Build failed"
    exit 1
fi
