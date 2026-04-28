#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e

# Change to the root directory of the project
cd "$(dirname "$0")/.."

# Create build directory if it doesn't exist
mkdir -p build

# Navigate into the build directory
cd build

# Run CMake to configure the project
echo "Configuring project with CMake..."
cmake ..

# Build the project using the available hardware threads
echo "Building project..."
make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

echo "Build complete. Executable is located at: build/AesEncryptorApp"
