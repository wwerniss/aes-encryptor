#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e

# Change to the root directory of the project
cd "$(dirname "$0")/.."

# Ensure the project is built first
./tools/build.sh

echo "Running tests..."
cd build
ctest --output-on-failure
