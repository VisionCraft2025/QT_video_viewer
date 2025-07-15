# Factory Video Client

Qt6 기반의 공장 비디오 클라이언트 애플리케이션입니다. 서버에서 에러 로그와 연관된 비디오를 조회하고 재생할 수 있습니다.

## 주요 기능

- **비디오 목록 조회**: 디바이스별, 에러 ID별, 시간 범위별 비디오 검색
- **비디오 다운로드**: HTTP를 통한 비디오 파일 다운로드 (진행률 표시)
- **비디오 재생**: Qt6 Multimedia를 사용한 로컬 비디오 재생
- **사용자 친화적 UI**: 직관적인 GUI 인터페이스

## 시스템 요구사항

- Ubuntu 20.04+ (또는 호환 Linux 배포판)
- Qt6 Base Development packages
- CMake 3.16+
- GCC/G++ 컴파일러
- 최소 4GB RAM

## 빌드 방법

### 1. 사전 준비

로컬 Qt6 Multimedia 라이브러리가 필요합니다. 시스템 Qt6 multimedia 패키지로는 빌드가 되지 않아 직접 소스에서 빌드해야 합니다.

#### 1.1. 라이브러리 소스 다운로드

먼저 필요한 Qt6 모듈들을 클론합니다:

```bash
# 라이브러리 디렉토리 생성
mkdir -p ~/dev/cpp_libs
cd ~/dev/cpp_libs

# Qt6 ShaderTools 클론 (qtmultimedia의 의존성)
git clone https://github.com/qt/qtshadertools.git
cd qtshadertools
git checkout 6.4.2  # 안정 버전 사용
cd ..

# Qt6 Multimedia 클론
git clone https://github.com/qt/qtmultimedia.git
cd qtmultimedia
git checkout 6.4.2  # 안정 버전 사용
cd ..
```

#### 1.2. Qt6 ShaderTools 빌드 (필수 선행 작업)

qtmultimedia는 qtshadertools에 의존하므로 **반드시 먼저** 빌드해야 합니다:

```bash
cd ~/dev/cpp_libs/qtshadertools

# 빌드 디렉토리 생성
rm -rf build
mkdir build && cd build

# CMake 설정 (install 디렉토리를 ../install로 지정)
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install

# 빌드 및 설치 (시간이 오래 걸림)
make -j$(nproc)
make install

# 설치 확인
ls ../install/lib/x86_64-linux-gnu/cmake/Qt6ShaderTools/
# Qt6ShaderToolsConfig.cmake 파일이 있어야 함
```

#### 1.3. Qt6 Multimedia 빌드

ShaderTools가 설치된 후 qtmultimedia를 빌드합니다:

```bash
cd ~/dev/cpp_libs/qtmultimedia

# 빌드 디렉토리 생성
rm -rf build
mkdir build && cd build

# 중요: LD_LIBRARY_PATH 설정 (qsb 도구가 libQt6ShaderTools.so를 찾을 수 있도록)
export LD_LIBRARY_PATH="$HOME/dev/cpp_libs/qtshadertools/install/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH"

# CMake 설정 (ShaderTools 경로 지정)
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=../install \
    -DQt6ShaderTools_DIR="$HOME/dev/cpp_libs/qtshadertools/install/lib/x86_64-linux-gnu/cmake/Qt6ShaderTools"

# 빌드 및 설치 (매우 오래 걸림, 30분+)
make -j$(nproc)
make install

# 설치 확인
ls ../install/lib/x86_64-linux-gnu/cmake/Qt6Multimedia/
# Qt6MultimediaConfig.cmake 파일이 있어야 함
```

#### 1.4. 빌드 완료 확인

최종적으로 다음 구조가 생성되어야 합니다:

```
~/dev/cpp_libs/
├── qtshadertools/
│   ├── build/
│   └── install/
│       └── lib/x86_64-linux-gnu/
│           ├── cmake/Qt6ShaderTools/
│           ├── libQt6ShaderTools.so*
│           └── qt6/bin/qsb*
└── qtmultimedia/
    ├── build/
    └── install/
        └── lib/x86_64-linux-gnu/
            ├── cmake/Qt6Multimedia/
            ├── cmake/Qt6MultimediaWidgets/
            ├── libQt6Multimedia.so*
            └── libQt6MultimediaWidgets.so*
```

#### 1.5. 빌드 중 발생 가능한 문제들

**문제 1: qsb 도구 실행 실패**
```
/home/user/dev/cpp_libs/qtshadertools/install/lib/qt6/bin/qsb: error while loading shared libraries: libQt6ShaderTools.so.6: cannot open shared object file
```
**해결:** LD_LIBRARY_PATH 설정 후 qtmultimedia 빌드

**문제 2: ShaderTools를 찾을 수 없음**
```
Could NOT find Qt6ShaderTools (missing: Qt6ShaderTools_DIR)
```
**해결:** qtshadertools를 먼저 완전히 빌드하고 설치한 후 qtmultimedia 빌드

**문제 3: 시스템 Qt6와 충돌**
```
Failed to find required Qt component "ShaderTools"
```
**해결:** CMAKE_PREFIX_PATH에서 로컬 빌드가 우선되도록 설정

**참고:** 전체 빌드 과정은 시스템 사양에 따라 1-2시간 소요될 수 있습니다.

### 2. 비디오 클라이언트 빌드

#### 2.1. x86-64 빌드 (데스크톱용)

```bash
# 실행 권한 부여
chmod +x build.sh

# 빌드 실행
./build.sh
```

#### 2.2. ARM64 빌드 (라즈베리파이용)

```bash
# 실행 권한 부여
chmod +x build_rpi.sh

# 빌드 실행
./build_rpi.sh
```

#### 2.3. 수동 빌드

```bash
# 의존성 설치
sudo apt update
sudo apt install -y qt6-base-dev qt6-base-dev-tools cmake build-essential pkg-config

# 빌드 디렉토리 생성
mkdir build && cd build

# CMake 설정
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DQt6Multimedia_DIR="$HOME/dev/cpp_libs/qtmultimedia/install/lib/x86_64-linux-gnu/cmake/Qt6Multimedia" \
    -DQt6MultimediaWidgets_DIR="$HOME/dev/cpp_libs/qtmultimedia/install/lib/x86_64-linux-gnu/cmake/Qt6MultimediaWidgets"

# 컴파일
make -j$(nproc)
```

### 3. 실행 방법

빌드 완료 후:

```bash
cd build

# 라이브러리 경로 설정하여 실행
LD_LIBRARY_PATH="$HOME/dev/cpp_libs/qtmultimedia/install/lib/x86_64-linux-gnu:$HOME/dev/cpp_libs/qtshadertools/install/lib/x86_64-linux-gnu" ./video_client
```

## 4. 빌드 문제 해결

### 1. Qt6Multimedia를 찾을 수 없는 경우

**에러:**
```
Could NOT find Qt6Multimedia (missing: Qt6Multimedia_DIR)
```

**해결법:**
- `~/dev/cpp_libs/qtmultimedia`가 제대로 빌드되고 설치되었는지 확인
- CMake에 정확한 경로 지정:
```bash
-DQt6Multimedia_DIR="$HOME/dev/cpp_libs/qtmultimedia/install/lib/x86_64-linux-gnu/cmake/Qt6Multimedia"
```

### 2. Qt6ShaderTools를 찾을 수 없는 경우

**에러:**
```
Could NOT find Qt6ShaderTools (missing: Qt6ShaderTools_DIR)
```

**해결법:**
- qtshadertools를 먼저 빌드하고 설치:
```bash
cd ~/dev/cpp_libs/qtshadertools
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../install
make -j$(nproc) && make install
```

### 3. 공유 라이브러리를 찾을 수 없는 경우

**에러:**
```
error while loading shared libraries: libQt6ShaderTools.so.6: cannot open shared object file
```

**해결법:**
- LD_LIBRARY_PATH 환경변수 설정:
```bash
export LD_LIBRARY_PATH="$HOME/dev/cpp_libs/qtmultimedia/install/lib/x86_64-linux-gnu:$HOME/dev/cpp_libs/qtshadertools/install/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH"
```

### 4. MOC 파일 관련 에러

**에러:**
```
fatal error: video_client_functions.moc: 그런 파일이나 디렉터리가 없습니다
```

**해결법:**
- 불필요한 `#include "*.moc"` 라인 제거 (이미 수정됨)
- 빌드 디렉토리 정리 후 재빌드:
```bash
rm -rf build && mkdir build
```

### 5. 중복 정의 에러

**에러:**
```
multiple definition of 'VideoClientExample::populateVideoList(...)'
```

**해결법:**
- 헤더 파일의 함수들을 `inline`으로 선언 (이미 수정됨)

### 6. 시스템 Qt6 패키지 충돌

**에러:**
시스템 Qt6와 로컬 빌드 Qt6 간의 충돌

**해결법:**
- 시스템 Qt6 multimedia 패키지 제거:
```bash
sudo apt remove qt6-multimedia-dev libqt6multimedia6-dev
```
- 또는 CMAKE_PREFIX_PATH 우선순위 조정

## 5. 디렉토리 구조

```
client/
├── build.sh              # x86-64 빌드 스크립트
├── build_rpi.sh          # ARM64 빌드 스크립트
├── CMakeLists.txt        # CMake 설정
├── main.cpp              # 메인 함수
├── mainwindow.cpp        # 메인 윈도우 구현
├── mainwindow.h          # 메인 윈도우 헤더
├── video_client_functions.hpp  # 비디오 클라이언트 기능
└── README.md             # 이 파일
```

## 6. 개발 환경 설정

### Qt Creator 사용시

1. Qt Creator에서 프로젝트 열기
2. Build Settings에서 CMake 설정:
   - `CMAKE_PREFIX_PATH`: `~/dev/cpp_libs/qtmultimedia/install;~/dev/cpp_libs/qtshadertools/install`
3. Run Settings에서 환경변수 설정:
   - `LD_LIBRARY_PATH`: `~/dev/cpp_libs/qtmultimedia/install/lib/x86_64-linux-gnu:~/dev/cpp_libs/qtshadertools/install/lib/x86_64-linux-gnu`

### VSCode 사용시

`.vscode/settings.json`:
```json
{
    "cmake.configureArgs": [
        "-DQt6Multimedia_DIR=${env:HOME}/dev/cpp_libs/qtmultimedia/install/lib/x86_64-linux-gnu/cmake/Qt6Multimedia",
        "-DQt6MultimediaWidgets_DIR=${env:HOME}/dev/cpp_libs/qtmultimedia/install/lib/x86_64-linux-gnu/cmake/Qt6MultimediaWidgets"
    ]
}
```

## 라이센스

이 프로젝트는 MIT 라이센스 하에 배포됩니다.

## 기여하기

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 문제 신고

버그나 기능 요청은 GitHub Issues를 통해 신고해 주세요.