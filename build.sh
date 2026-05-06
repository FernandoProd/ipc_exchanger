#!/bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"

echo "==> Cleaning previous building..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

echo "==> Configuration (Debug)..."
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE=Debug

echo "==> Building..."
cmake --build .

echo "==> Ready! Executable files:"
ls -l sender receiver