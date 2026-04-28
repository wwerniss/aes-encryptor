#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e

# Change to the root directory of the project
cd "$(dirname "$0")/.."

APP_PATH="build/AesEncryptorApp"

# Check if the executable exists
if [ ! -f "$APP_PATH" ]; then
    echo "Executable not found at $APP_PATH!"
    echo "Please run ./tools/build.sh first."
    exit 1
fi

echo "Launching AES Encryptor Application..."
./"$APP_PATH"
