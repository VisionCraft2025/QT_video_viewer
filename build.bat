@echo off
REM Windows용 빌드 스크립트

set CPP_LIBS_DIR=%USERPROFILE%\dev\cpp_libs
set PROJECT_DIR=%~dp0

echo Building video client for Windows...
cd /d "%PROJECT_DIR%"
if exist build rmdir /s /q build
mkdir build
cd build

REM Windows용 CMake 설정
cmake .. -DCMAKE_BUILD_TYPE=Release ^
    -DQt6Multimedia_DIR="%CPP_LIBS_DIR%\qtmultimedia\install\lib\cmake\Qt6Multimedia" ^
    -DQt6MultimediaWidgets_DIR="%CPP_LIBS_DIR%\qtmultimedia\install\lib\cmake\Qt6MultimediaWidgets" ^
    -DQt6Mqtt_DIR="%CPP_LIBS_DIR%\qtmqtt\install\lib\cmake\Qt6Mqtt"

cmake --build . --config Release

if %ERRORLEVEL% EQU 0 (
    echo Build completed successfully!
    echo Run with: video_client.exe
) else (
    echo Build failed!
    exit /b 1
)