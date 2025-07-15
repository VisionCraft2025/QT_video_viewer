#!/bin/bash

# 라즈베리파이용 빌드 스크립트

echo "Installing Qt6 dependencies for Raspberry Pi..."
sudo apt update
sudo apt install -y qt6-base-dev qt6-multimedia-dev qt6-wayland-dev
sudo apt install -y cmake build-essential pkg-config

echo "Building video client..."
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_DIR"
mkdir -p build
cd build

# ARM64 최적화 플래그
export CXXFLAGS="-O2 -march=armv8-a"
export CFLAGS="-O2 -march=armv8-a"

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo "Build completed. Run with: ./video_client"