#!/bin/bash
# Build script for Windows (static build)

set -e

echo "🔨 Building Antidetect Browser for Windows..."

# Check if MinGW is installed
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo "❌ MinGW not found. Installing..."
    sudo apt-get update
    sudo apt-get install -y mingw-w64 cmake
fi

# Create build directory
BUILD_DIR="build-windows"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake for Windows cross-compilation
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../windows-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXE_LINKER_FLAGS="-static -static-libgcc -static-libstdc++" \
    -DQt6_DIR="/usr/x86_64-w64-mingw32/lib/cmake/Qt6"

# Build
cmake --build . -j$(nproc)

echo "✅ Build complete!"
echo "📦 Executable: $BUILD_DIR/AntidetectBrowser.exe"
