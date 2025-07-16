# Windows 빌드 가이드

## Windows에서 빌드하기

### 1. 사전 준비

- Visual Studio 2019+ 또는 Visual Studio Build Tools
- CMake 3.16+
- Git for Windows

### 2. Qt6 라이브러리 빌드 (Windows)

```cmd
REM 라이브러리 디렉토리 생성
mkdir %USERPROFILE%\dev\cpp_libs
cd /d %USERPROFILE%\dev\cpp_libs

REM Qt6 ShaderTools 클론
git clone https://github.com/qt/qtshadertools.git
cd qtshadertools
git checkout 6.4.2
cd ..

REM Qt6 Multimedia 클론
git clone https://github.com/qt/qtmultimedia.git
cd qtmultimedia
git checkout 6.4.2
cd ..
```

### 3. ShaderTools 빌드

```cmd
cd %USERPROFILE%\dev\cpp_libs\qtshadertools
mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install
cmake --build . --config Release
cmake --install . --config Release
```

### 4. Multimedia 빌드

```cmd
cd %USERPROFILE%\dev\cpp_libs\qtmultimedia
mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install ^
    -DQt6ShaderTools_DIR="%USERPROFILE%\dev\cpp_libs\qtshadertools\install\lib\cmake\Qt6ShaderTools"

cmake --build . --config Release
cmake --install . --config Release
```

### 5. 비디오 클라이언트 빌드

```cmd
REM 크로스 플랫폼 CMakeLists.txt 사용
copy CMakeLists_cross_platform.txt CMakeLists.txt

REM 빌드 실행
build.bat
```

### 6. 실행

빌드 완료 후 `build/Release/video_client.exe` 실행

## 주요 차이점

- **경로**: `%USERPROFILE%` 사용 (Linux의 `$HOME` 대신)
- **라이브러리 경로**: `lib/cmake` (Linux의 `lib/x86_64-linux-gnu/cmake` 대신)
- **실행파일**: `.exe` 확장자
- **빌드 도구**: Visual Studio 컴파일러 사용