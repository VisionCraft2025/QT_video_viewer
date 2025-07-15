#!/bin/bash

# 간단한 시스템 Qt6 사용 빌드

echo "Installing Qt6 multimedia packages..."
sudo apt update
sudo apt install -y qt6-multimedia-dev libqt6multimedia6-dev

echo "Building video client..."
cd /home/kwon/Programming/DB/video/db_video_mqtt/client
rm -rf build
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "Build completed successfully. Run with: ./video_client"
else
    echo "Build failed!"
    exit 1
fi