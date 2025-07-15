#!/bin/bash

# x86-64용 빌드 스크립트

CPP_LIBS_DIR="$HOME/dev/cpp_libs"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Using local Qt6 libraries..."

echo "Installing system Qt6 dependencies..."
sudo apt update
sudo apt install -y qt6-base-dev qt6-base-dev-tools cmake build-essential pkg-config

echo "Building video client..."
cd "$PROJECT_DIR"
rm -rf build
mkdir build
cd build

# 라이브러리 경로 설정
export LD_LIBRARY_PATH="${CPP_LIBS_DIR}/qtmultimedia/install/lib/x86_64-linux-gnu:${CPP_LIBS_DIR}/qtshadertools/install/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH"

# x86-64 최적화 플래그
export CXXFLAGS="-O2 -march=x86-64"
export CFLAGS="-O2 -march=x86-64"

cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DQt6Multimedia_DIR="${CPP_LIBS_DIR}/qtmultimedia/install/lib/x86_64-linux-gnu/cmake/Qt6Multimedia" \
    -DQt6MultimediaWidgets_DIR="${CPP_LIBS_DIR}/qtmultimedia/install/lib/x86_64-linux-gnu/cmake/Qt6MultimediaWidgets"

make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "Build completed successfully!"
    echo "Run with: LD_LIBRARY_PATH=${CPP_LIBS_DIR}/qtmultimedia/install/lib/x86_64-linux-gnu:${CPP_LIBS_DIR}/qtshadertools/install/lib/x86_64-linux-gnu ./video_client"
else
    echo "Build failed!"
    exit 1
fi